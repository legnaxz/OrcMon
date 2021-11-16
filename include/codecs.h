#ifndef _CODECS_H_
#define _CODECS_H_

#include "common.h"

namespace orc {

class Codecs {

    class Private;
    friend class Codecs::Private;
    typedef Private* PrivatePtr;

public: 
    Codecs();
    ~Codecs();

public:
    int wavSplit( const std::string path, const std::string filename );

private:
    PrivatePtr private_;
};

};

#endif // _CODECS_H_