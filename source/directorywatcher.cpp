#include "directorywatcher.h"

#include <map>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <stdexcept>

#include <log4cxx/logger.h>

#include <boost/thread/thread.hpp>

#include "manager.h"

#define BUFF_SIZE ((sizeof(struct inotify_event)+FILENAME_MAX)*1024)

namespace orc {

class Exception : public std::runtime_error
{
public:
    Exception( const std::string& message )
        : std::runtime_error( message )
    {}
};

class FileNotFoundException : public Exception
{
public:
    FileNotFoundException()
        : Exception( "File not found" )
    {}

    FileNotFoundException( const std::string& filename )
        : Exception( "File not found (" + std::string(filename.begin(), filename.end()) + ")" )
    {}
};    

class DirectoryWatcher::Private
{
    struct WatchStruct
    {
        unsigned long watch_id_;
        std::string dir_path_;
        DirectoryWatchListener* listener_;
    };

    typedef std::map<unsigned long, WatchStruct*> WatchMap;

    log4cxx::LoggerPtr Logger_ = log4cxx::Logger::getLogger( "DirectoryWatcher" );

public:
    Private( DirectoryWatcher* directory_watcher ) 
        : directory_watcher_( directory_watcher )
    {
        fd_ = inotify_init();
        if ( fd_ < 0 )
        {
            LOG4CXX_DEBUG( Logger_ , "Error : " << strerror( errno ) );
        }

        timeout_.tv_sec = 0;
        timeout_.tv_usec = 0;

        FD_ZERO( &descriptor_ );
    }

    Private( void ) = default;

    ~Private()
    {
        WatchMap::iterator iter = watches_.begin();
        WatchMap::iterator end = watches_.end();
        for(; iter != end; ++ iter ) 
        {
            delete iter->second;
        }
        watches_.clear();
    }

    inline unsigned long addWatch( const std::string& directory, DirectoryWatchListener* watch_listener, bool recursive )
    {
        int wd = inotify_add_watch ( fd_, directory.c_str(), 
			IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE | IN_MOVED_FROM | IN_DELETE );

		if ( wd < 0 )
		{
			if( errno == ENOENT )
				throw FileNotFoundException( directory );
			else
				throw Exception(strerror( errno ) );
		}
		
		WatchStruct* watch_ptr = new WatchStruct();
		watch_ptr->listener_ = watch_listener;
		watch_ptr->watch_id_ = wd;
		watch_ptr->dir_path_ = directory;
		
		watches_.insert( std::make_pair( wd, watch_ptr ) );
	
		return wd;
    }

    inline void removeWatch( const std::string& dir_path )
    {
        WatchMap::iterator iter = watches_.begin();
        WatchMap::iterator end = watches_.end();
        for(; iter != end; ++ iter ) 
        {
            if( dir_path == iter->second->dir_path_ )
            {
                removeWatch( iter->first );
                return;
            }
        }
    }

    inline void removeWatch( unsigned long watch_id )
    {
        WatchMap::iterator iter = watches_.find( watch_id );

        if (  iter == watches_.end() )
            return ;
        
        WatchStruct* watch = iter->second;
        watches_.erase( iter );

        inotify_rm_watch( fd_, watch_id );

        delete watch;
        watch = 0;
    }

    inline void update()
    {
        int ret;
        while( 1 ) {
            FD_SET( fd_, &descriptor_ );

            ret = select( fd_ + 1, &descriptor_, NULL, NULL, &timeout_ );

            if( ret < 0 )
            {
                LOG4CXX_DEBUG( Logger_, "select" );
            }
            else if ( FD_ISSET( fd_, &descriptor_) )
            {
                ssize_t len, i = 0;
                char action[81 + FILENAME_MAX]= {0};
                char buff[BUFF_SIZE] = {0};

                len = read( fd_, buff, BUFF_SIZE );

                while( i < len )
                {
                    struct inotify_event *pevent = (struct inotify_event *)&buff[i];

                    WatchStruct* watch = watches_[pevent->wd];
                    handleAction( watch, pevent->name, pevent->mask );
                    i += sizeof( struct inotify_event ) + pevent->len;
                }
            }
        }
    }

    inline void handleAction( WatchStruct* watch, const std::string& filename, unsigned long action )
    {
        if( !watch->listener_ )
        {
            return;    
        }

        if ( IN_CLOSE_WRITE & action )
        {
            watch->listener_->handleFileAction( watch->watch_id_, watch->dir_path_, filename, Action::Modified );
        }

        if( IN_MOVED_TO & action || IN_CREATE & action )
        {
            watch->listener_->handleFileAction( watch->watch_id_, watch->dir_path_, filename, Action::Add );
        }

        if( IN_MOVED_FROM & action || IN_DELETE & action )
        {
            watch->listener_->handleFileAction( watch->watch_id_, watch->dir_path_, filename, Action::Delete );
        }
    }

    void start(void) {
        thread_ = boost::thread( boost::bind ( &DirectoryWatcher::update, directory_watcher_ ) ); 
    }
private:

    DirectoryWatcher* directory_watcher_;
    WatchMap watches_;
    unsigned long last_watch_id_;
    int fd_;
    struct timeval timeout_;
    fd_set descriptor_;

    boost::thread thread_;

};

DirectoryWatcher::DirectoryWatcher()
    : private_( new Private( this ) )
{
   
}

DirectoryWatcher::~DirectoryWatcher()
{
    private_->~Private();
    delete private_;
}

unsigned long DirectoryWatcher::addWatch( const std::string& dir_path, DirectoryWatchListener* watch_listener, bool recursive )
{
    return private_->addWatch( dir_path, watch_listener, recursive );
}

void DirectoryWatcher::removeWatch( const std::string& dir_path )
{
    private_->removeWatch( dir_path );
}

void DirectoryWatcher::update()
{
    private_->update();
}

void DirectoryWatcher::start()
{
    private_->start();
}

};