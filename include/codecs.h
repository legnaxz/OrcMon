#ifndef _CODECS_H_
#define _CODECS_H_

//Todo
//Open wavfile
//Open 0, 1 ch file
//Convert wav to pcm chs
//write file.
//save.
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
    int wavSplit( const std::string input_path, const std::string output_path );

private:
    PrivatePtr private_;

};

};

#endif // _CODECS_H_