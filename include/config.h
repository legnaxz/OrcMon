#ifndef _ORC_CONFIG_H_
#define _ORC_CONFIG_H_

#include "common.h"

#include "singletone.h"

namespace orc
{
    class Config : public Singleton<Config>
    {
        class Private;
        friend class Config::Private;
        typedef Private *PrivatePtr;

        friend class Singleton<Config>;

        friend class Manager;


    public:
        class SubSet
        {
            friend class Config::Private;
            typedef Config::Private *PrivatePtr;

        public:
            SubSet(const std::string &key);
            virtual ~SubSet(void);

        protected:
            bool number(const std::string &key, int32_t &value) const;
            bool number(const std::string &key, uint32_t &value) const;
            bool number(const std::string &key, int64_t &value) const;
            bool number(const std::string &key, uint64_t &value) const;
            bool number(const std::string &key, float &value) const;
            bool number(const std::string &key, double &value) const;
            bool boolean(const std::string &key, bool &value) const;
            bool string(const std::string &key, std::string &value) const;
            bool stringList(const std::string &key, StringList &string_list) const;
            bool keyList(StringList &string_list) const;
            bool objectStream(const std::string &key, std::string &object_stream) const;
            bool bind(Config::PrivatePtr private_ptr);

        protected:
            virtual bool bind(void) = 0;

        public:
            bool bindSubSet(SubSet &sub_set);
            const std::string &key(void) const
            {
                return key_;
            }

        private:
            const std::string key_;
            PrivatePtr private_;
        };

    private:
        Config(void);

    public:
        ~Config(void);

    public:
        bool bindSubSet(SubSet &sub_set);

    private:
        bool load(const std::string &path);
        PrivatePtr private_;

    };
}

#endif