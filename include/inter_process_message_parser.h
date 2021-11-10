#ifndef _INTER_PROCESS_MESSAGE_PARSER_H_
#define _INTER_PROCESS_MESSAGE_PARSER_H_

#include "stdtypes.h"

namespace orc{

class InterProcessMessageParser {
    class Private;
    friend class InterProcessMessageParser::Private;
    typedef Private* PrivatePtr;

public:
    InterProcessMessageParser( void );
    virtual ~InterProcessMessageParser( void );

private:
    PrivatPtr* private_;
};

}

#endif // _INTER_PROCESS_MESSAGE_PARSER_H_
