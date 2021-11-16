#ifndef _DIRECTORY_WATCH_LISTENER_H_
#define _DIRECTORY_WATCH_LISTENER_H_

#include "common.h"
#include "boost/thread.hpp"
#include "codecs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include "tcp_client.h"

namespace orc {

typedef Actions::Action Action;

class DirectoryWatchListener
{

public:
    DirectoryWatchListener( const std::string& base_dir, const std::string& intermediate_dir );
    ~DirectoryWatchListener();
    void handleFileAction( unsigned long id, const std::string& filenamem, Action action );
    void handleWrite( const boost::system::error_code &e );

public:
    inline std::string removeExtension( const std::string filename )
    {
        size_t lastdot = filename.find_last_of(".");
        if ( lastdot == std::string::npos ) {
            return filename;
        }
        return filename.substr( 0, lastdot );
    }

    inline bool isExist( const std::string path )
    {
        if ( stat( path.c_str(), &info_ ) != 0 )
            return false;
        else
            return true; 
    }

private:
    Codecs codec_;
    boost::thread thread_;

    long file_count_ = 0;

    std::string intermediate_dir_;
    std::string base_dir_;

    struct stat info_;

    boost::mutex mutex_;
    boost::asio::io_service io_service_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_;
    std::ofstream transfer_file_log_;

};

};

#endif // _DIRECTORY_WATCH_LISTENER_H_