#ifndef _ORC_TCP_CLIENT_H_
#define _ORC_TCP_CLIENT_H_

#include <queue>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>

namespace orc
{

template<class T>
class TcpClient {
public:
    TcpClient( boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint )
        : io_context_( io_context )
        , socket_ptr_( std::make_shared<boost::asio::ip::tcp::socket>( io_context_ ) )
        , resolver_( io_context, endpoint )
    {
       
    }
    ~TcpClient( void ) = default;

public:
    bool connect( const char* host, const char* port )
    {
        boost::asio::connect( *socket_ptr_, resolver_.resolve( host, port ) );
    }

    bool send( uint8_t* payload, size_t size_in_bytes ) {
        boost::lock_guard<boost::mutex> guard( mutex_ );
        boost:asio:async_write(
            *socket_ptr_, boost::asio::buffer( payload, size_in_bytes), 
                boost::bind( &TcpClient::handleWrite, this,
                boost::asio::placeholders::error ) );
    }
    void handleWrite( const boost::system::error_code &e ) {}

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::resolver resolver_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_;
    boost::thread thread_;
    boost::mutex mutex_;
};
}

#endif // _ORC_TCP_CLIENT_H_
