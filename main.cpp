
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <jsoncpp/json/json.h>
#include <utility>
#include <fstream>
#include <stdlib.h>

#include "manager.h"
#include "version.h"
#include "directory_watcher.h"
#include "directory_watch_listener.h"

#include "codecs.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include <signal.h>

#include <unistd.h>

#define LOG4CXX_CONFIG_FILE "./log4cxx.conf"

using namespace std;

int main ( int argc, char ** argv )
{
    try
    {
        orc::DirectoryWatcher directory_watcher;

        log4cxx::PropertyConfigurator::configure( log4cxx::File( LOG4CXX_CONFIG_FILE ) );
        log4cxx::LoggerPtr logger( log4cxx::Logger::getLogger( "orc" ) );

        LOG4CXX_INFO( logger, "yespeech ORC starting ... (ver " << ORC_VERSION << ")" );

        signal( SIGPIPE, SIG_IGN );

        orc::Manager::instance().initialize( "orc_config.json" );

        LOG4CXX_DEBUG( logger, "Press ^C to exit demo" );

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}