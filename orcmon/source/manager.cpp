#include "manager.h"

#include "config.h"

#include "log4cxx/logger.h"

#include <deque>

#include <queue>

#include <boost/thread.hpp>

namespace orc {
    Manager::OrcConfigFileInfo::OrcConfigFileInfo( void )
        : Config::SubSet( "orc_config_file" )
    {}

    bool Manager::OrcConfigFileInfo::bind( void )
    {
        if ( false ==  string( "version", this->version_) ) {
            version_.clear();
            return false;
        } 

        return true;  
    }

    Manager::OrcConfig::OrcConfig( void )
        : Config::SubSet( "orc" )
    {}

    bool Manager::OrcConfig::bind( void ) {
        //.. bind classes;

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
                //, paaket_queue_( new PacketQueue( 214u ) )
                //, rtp_packet_router_( nullptr )
            {}

            ~Private( void ) = default;

        public:
            inline void waitFoever( void ) {
                //rpt_packet_router->waitForever();
            }

        private:
            Manager* manager_;
            boost::mutex mutex_;

    };

    log4cxx::LoggerPtr Manager::Private::Logger_ = log4cxx::Logger::getLogger("Manager");

    Manager::Manager( void )
        : private_( new Private( this ))
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

        LOG4CXX_INFO( Private::Logger_, "Load configuration from \"" << configFilePath << "\": " <<
                                            orc_config_file_info_.key() << "=" << orc_config_file_info_.version() );

        return true;
    }

    void Manager::waitForever( void )
    {
        private_->waitFoever();
    }
}