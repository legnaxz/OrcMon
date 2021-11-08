#include "manager.h"

#include "config.h"

#include "log4cxx/logger.h"

#include <deque>

#include <queue>

#include <boost/thread.hpp>

#include <iostream>

namespace orc {

class UpdateListener : public orc::DirectoryWatchListener
{
    public:
        UpdateListener() {}
        void handleFileAction( unsigned long watchid, const std::string& dir, const std::string& filename, orc::Action action )
        {
            if ( filename.find( ".wav" ) >= 0 && orc::Actions::Add == action)
            {
                std::cout << "a__DIR (" << dir + ") FILE (" + filename + ") has event " << action << std::endl;
                orc::Codecs decode;
                decode.wavSplit( dir, filename );
            }
        }
};

Manager::OrcConfigFileInfo::OrcConfigFileInfo( void )
    : Config::SubSet( "orc_config_file" )
{}

bool Manager::OrcConfigFileInfo::bind( void )
{
    if ( false ==  string( "version", this->version_ ) ) {
        version_.clear();
        return false;
    } 

    return true;  
}

Manager::OrcConfig::OrcConfig( void )
    : Config::SubSet( "orc" )
{}

bool Manager::OrcConfig::bind( void ) {

    if ( false == bindSubSet( watcher_ ) ) {
        std::cout << "Can not load configuration form \"" << watcher_.key() << "\"";
        return false;
    }

    if ( false == bindSubSet( inter_process_ ) ) {
        std::cout << "Can not load configuration form \"" << inter_process_.key() << "\"";
        return false;
    }

    if ( false == bindSubSet( process_fd_ ) ) {
        std::cout << "Can not load configuration form \"" << process_fd_.key() << "\"";
        return false;
    }

    return true;

}

Manager::OrcConfig::Watcher::Watcher( void )
    : Config::SubSet( "watcher" )
{}

bool Manager::OrcConfig::Watcher::bind( void )
{
    if ( false == string ( "base_path", base_path_) ) {
        base_path_.clear();
        return false;
    }

    if ( false == string ( "export_path", export_path_) ) {
        export_path_.clear();
        return false;
    }

    return true;
}

Manager::OrcConfig::InterProcess::InterProcess( void )
    : Config::SubSet( "inter_process" ) 
    , port_ ( 0 )
{}

bool Manager::OrcConfig::InterProcess::bind( void )
{
    uint32_t port_number;

    if ( false == string ( "mod", mode_ ) )
    {
        mode_.clear();
        return false;
    }

    if ( false == string ( "ip", ip_ ) )
    {
        ip_.clear();
        return false;
    }

    if ( false == number ( "port", port_number ) )
    {
        port_ = 0;
        return false;
    }

    port_ = static_cast<uint16_t>( port_number );

    return true;
}

Manager::OrcConfig::ProcessFD::ProcessFD( void )
    : Config::SubSet( "pfd" ) 
    , max_childs_( 1 )
{}

bool Manager::OrcConfig::ProcessFD::bind( void )
{
    uint32_t max_childs = 0;

    if ( false ==  number( "max_childs", max_childs ) )
    {
        max_childs = 0;
        return false;
    }

    max_childs_ = static_cast<uint16_t>( max_childs );

    return true;
}

class Manager::Private
{
    //typedef std::queue<int32_t> AvailableCallSessions;
    //typedef std::vector<Callsession*> CallSesssions;

    public:
        static log4cxx::LoggerPtr Logger_;

    public: 
        Private( Manager* manager )
            : manager_( manager )
            , directory_watcher_( nullptr )
            , listener_( nullptr )
        {}

        ~Private( void ) = default;

    public:
        inline void waitFoever( void ) {
            //rpt_packet_router->waitForever();
        }

        inline DirectoryWatcher* directoryWatcher( void ) {
            return directory_watcher_;
        }

        inline void bindDirectoryWatcher( DirectoryWatcher* directory_watcher )
        {
            directory_watcher_ = directory_watcher;
        }

        inline UpdateListener* updatelistenr( void ) {
            return listener_;
        }

        inline void bindUpdateListener( UpdateListener* listener )
        {
            listener_ = listener;
        }

        inline void addWatch( const std::string base_dir )
        {
            directory_watcher_->addWatch( base_dir, listener_, false );
        }

        inline void watcher_start( void )
        {
            directory_watcher_->start();
        }

    private:
        Manager* manager_;
        boost::mutex mutex_;

        DirectoryWatcher *directory_watcher_;
        UpdateListener *listener_;

        unsigned long base_watch_id_;

};

log4cxx::LoggerPtr Manager::Private::Logger_ = log4cxx::Logger::getLogger( "Manager" );

Manager::Manager( void )
    : private_( new Private( this ) )
{}

Manager::~Manager( void )
{
    delete private_;
}

bool Manager::initialize( const std::string& configFilePath )
{
    if ( false == Config::instance().load( configFilePath ) ) {
        LOG4CXX_FATAL( Private::Logger_, "Can not load configuration form \"" << configFilePath << "\"" );
        return false;
    }

    if( false == Config::instance().bindSubSet( orc_config_file_info_ ) ) {
        LOG4CXX_FATAL( Private::Logger_, "Can not load configuration sub-set for \"" << orc_config_file_info_.key() << "\"" );
        return false;
    }

    if( false == Config::instance().bindSubSet( orc_config_ ) ) {
        LOG4CXX_FATAL( Private::Logger_, "Can not load configuration sub-set for \"" << orc_config_.key() << "\"" );
        return false;
    }

    if ( !isExist( orcConfig().watcher().export_path() ) )
    {
        std::string mkdir_command = "mkdir " + orcConfig().watcher().export_path();
        std::cout << mkdir_command << std::endl;
        system( mkdir_command.c_str() );
    }

    LOG4CXX_INFO( Private::Logger_, "Load configuration from \"" << configFilePath << "\": " <<
                                        orc_config_file_info_.key() << "=" << orc_config_file_info_.version() );
    
    DirectoryWatcher* directory_watcher = new DirectoryWatcher();
    UpdateListener *update_listener = new UpdateListener();

    private_->bindDirectoryWatcher( directory_watcher );
    private_->bindUpdateListener( update_listener );
    private_->addWatch( orcConfig().watcher().base_path() );
    private_->watcher_start();

    return true;
}

std::string Manager::getBaseDir()
{
    return orc_config_.watcher().base_path();
}

void Manager::waitForever( void )
{
    private_->waitFoever();
}

} 