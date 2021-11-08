#ifndef _INTER_PROCESS_LISTENER_H_
#define _INTER_PROCESS_LISTENER_H_

#include <stdtype.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "log4cxx/logger.h"

namespace orc {

class InterProcessMessageParser;
class InterProcessListener : public std::enable_shared_form_this<InterProcessListener>
{
    static log4cxx::LoggerPtr Logger_;
public:
    InterProcessListener( boost::asio::ip::tcp::socket socket );
    virtual ~InterProcessListener( void );

public:
    void start();

private:
    void receiveHeader( void );
    void receivePayload( void );
    void close( void );

private:
    boost::asio::ip::tcp::socket socket_;
    InterProcessMessageParser* inter_process_message_parser_;
};

}

#endif // _INTER_PROCESS_LISTENER_H_
