#ifndef _INTER_PROCESS_MODE_H_
#define _INTER_PROCESS_MODE_H_

namespace orc { 
    enum class IneterProcessMode : uint32_t {
        kStandAlone  = 0,
        kActive      = 1,
        kStandby     = 2,
        kTemporary   = 3
    };
}

#endif // _INTER_PROCESS_MODE_H_