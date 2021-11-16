#ifndef _ORC_COMMON_H_
#define _ORC_COMMON_H_

#define ORC_DISABLE_COPY(Class)             \
    Class(const Class &) = delete;          \
    Class& operator=(const Class &) = delete

#define ORC_DISABLE_MOVE(Class)             \
    Class(Class&&) = delete;                \
    Class& operator=(Class&&) = delete

#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <sstream>
#include <map>

namespace orc
{
    namespace Actions
    {
        enum Action
        {
            Add = 1,
            Delete = 2,
            Modified = 4
        };
    };

    typedef std::vector<std::string> StringList;
    typedef std::vector<uint8_t *> BufferList;

    inline const char *booleanToString(bool boolean)
    {
        return boolean ? "true" : "false";
    }

    inline const std::string stringListToString(const StringList &string_list)
    {
        if (0 != string_list.size())
        {
            std::ostringstream ss;
            ss << string_list[0];

            for (StringList::size_type i = 1; i < string_list.size(); ++i)
            {
                ss << "," << string_list[i];
            }

            return ss.str();
        }

        return "";
    }
}

#endif //#ifndef _ORC_COMMON_H_