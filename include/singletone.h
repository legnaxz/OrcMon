#ifndef _ORC_SINGLETONE_H_
#define _ORC_SINGLETONE_H_

#include <common.h>

namespace orc
{
    template <typename T>
    class Singleton
    {
    protected:
        Singleton(void) = default;

    public:
        ORC_DISABLE_COPY(Singleton);
        ORC_DISABLE_MOVE(Singleton);

    public:
        static T &instance(void)
        {
            static T singleInstance;
            return singleInstance;
        }
    };

}

#endif // #ifndef _ORC_SINGLETONE_H_