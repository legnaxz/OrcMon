#include "inter_process_listener.h"

#include "inter_process_message_parser.h"


namespace orc 
{
    log4cxx::LoggerPtr InterProcessListener::Logger_ = log4cxx::Logger::getLogger(" InterProcessListener ");

    InterProcessListener::InterProcessListener( boost::asio::ip::tcp::socket socket )
        : socket_( std::move( socket ) )
        , inter_process_message_parser_( new InterProcessParser() )
    {}

    void InterProcessListener::start( void )
    {
        receiveHeader();
    }

    InterProcessListener::~InterProcessListener( void )
    {
        close();

        delete inter_process_message_parser_;
    }

    void InterProcessListener::receiveHeader( void )
    {
        auto self( shared_from_this() );

        boost::asio::async_read( socket_, )

    }

    void InterProcessListener::receivePayload( void )
    {
        auto self( shared_from_this() );

        
    }

    void InterProcessListner::close( void )
    {
        if ( false == socket_.is_open() ) {
            LOG4CXX_DEBUG( Logger_, "Close socket... ");
            socket_.close();
        }
    }


}
