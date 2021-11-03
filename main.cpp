#include <directorywatcher.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <jsoncpp/json/json.h>
#include <utility>
#include <fstream>

#include "manager.h"
#include "version.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include <signal.h>

#include <unistd.h>

#define LOG4CXX_CONFIG_FILE "./log4cxx.conf"

using namespace std;

class UpdateListener : public orc::DirectoryWatchListener
{
    public:
        UpdateListener() {}
        void handleFileAction( unsigned long watchid, const std::string& dir, const std::string& filename,
            orc::Action action )
        {
            std::cout << "DIR (" << dir + ") FILE (" + filename + ") has event " << action << std::endl;
        }
};

int main ( int argc, char ** argv )
{
    try
    {
        UpdateListener listener;

        orc::DirectoryWatcher directory_watcher;

        unsigned long watchID = directory_watcher.addWatch("/home/yes/sktstt/", &listener, true);

        log4cxx::PropertyConfigurator::configure( log4cxx::File( LOG4CXX_CONFIG_FILE ) );
        log4cxx::LoggerPtr logger( log4cxx::Logger::getLogger( "orc" ) );

        LOG4CXX_INFO( logger, "yespeech ORC starting ... (ver " << ORC_VERSION << ")" );

        signal( SIGPIPE, SIG_IGN );

        orc::Manager::instance().initialize( "orc_config.json" );

        std::cout << "Press ^C to exit demo" << std::endl;

		while(1)
		{
			directory_watcher.update();
		}
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}