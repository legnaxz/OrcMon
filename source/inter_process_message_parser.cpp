#include "inter_process_message_parser.h"

#include "inter_process_mode.h"
#include "manager.h"

#include <sstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <log4cxx/log4cxx.h>

namespace orc 
{

class InterProcessMessageParser : Private
{
    struct check_message
    {
        char sender[ 10 ];
        char text[ 10 ];
    };

    static log4cxx::LoggerPtr Logger_;

public:
    Private( InterProcessMessageParser* inter_process_message_parser_ )
};

}