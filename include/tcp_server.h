#ifndef _ORC_TCP_SERVER_H_
#define _ORC_TCP_SERVER_H_

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

namespace orc {

template<class T>
class TcpServer {
public:
    TcpServer( boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint )
        : io_context_( io_context )
        , acceptor_( io_context, endpoint )
    {
        listen();
    }
    ~TcpServer( void ) = default;

public:
    void start( uint16_t port_number ) {
        io_context_.run();
    }

public:
    void listen( void ) {
        acceptor_.async_accept([this]( boost::system::error_code error_code, boost::asio::ip::tcp::socket socket ) {
            if( false == error_code.failed() ) {
                std::make_shared<T>( std::move( socket ) )->start();
            }

            listen();
        });
     }

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

}

#endif // #ifndef _ORC_TCP_SERVER_H_
