#include "directory_watch_listener.h"

#include "codecs.h"
#include "manager.h"
#include <iostream>

namespace orc {

DirectoryWatchListener::DirectoryWatchListener( const std::string& base_dir, const std::string& intermediate_dir )
        : socket_ptr_( std::make_shared<boost::asio::ip::tcp::socket>( io_service_ ) )

{
        base_dir_ = base_dir;
        intermediate_dir_ = intermediate_dir;
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string( Manager::instance().orcConfig().processfd().child_ip() )
                                                , (int)Manager::instance().orcConfig().processfd().child_port() );
        try {
            socket_ptr_->connect( endpoint );
        } catch ( const std::exception e ){
            std::cout << e.what() << std::endl;
        }
    
}

DirectoryWatchListener::~DirectoryWatchListener(){}

void DirectoryWatchListener::handleFileAction( unsigned long watchid, const std::string& filename, orc::Action action )
{
    boost::lock_guard<boost::mutex> guard( mutex_ );

    if ( filename.find( ".wav" ) != std::string::npos && orc::Actions::Add == action)
    {
        std::cout << "a__DIR (" << base_dir_ + ") FILE (" + filename + ") has event " << action << std::endl;

        int finish_split = 0;
        int finish_bash = 0;
        std::string removel_filename = removeExtension(filename);
        std::string export_path = "/home/yes/destination/" + removel_filename + "/";
        
        std::string command_original_file_move = "mv " + base_dir_ + filename + " " + export_path;
        std::string command_splited_file_move = "mv " + intermediate_dir_  + removel_filename + "*" + " " + export_path;

        removel_filename += "|" + export_path + "|" + command_original_file_move + "|" + command_splited_file_move;

        finish_split = codec_.wavSplit( base_dir_, filename );
        
        // if ( !isExist( export_path ) )
        // {
        //     std::string mkdir_command = "mkdir " + export_path;
        //     std::cout << mkdir_command << std::endl;
        //     system( mkdir_command.c_str() );
        // }
        // finish_bash = system( command_original_file_move.c_str() );
        // finish_bash = system( command_splited_file_move.c_str() );
        
        if ( nullptr != socket_ptr_ ) {
            if ( socket_ptr_ ){
                boost:asio:async_write(
                    *socket_ptr_, boost::asio::buffer( removel_filename, removel_filename.length() ), boost::bind( &DirectoryWatchListener::handleWrite, this,
                        boost::asio::placeholders::error ) );
            } else {
                std::cout << "Connect Error !!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
            } 
        } else {
            std::cout << "Connect Error @@@@@@@@@@@@@@@@@@@@@@@@@@@ "<< std::endl;
        }

        file_count_++;
    }

    if ( filename.find( ".wav" ) != std::string::npos && orc::Actions::Delete == action )
    {
        std::cout << "a__DIR (" << base_dir_ + ") FILE (" + filename + ") has event " << action << std::endl;
        file_count_--;
    }

}

void DirectoryWatchListener::handleWrite( const boost::system::error_code &e )
{}

}

