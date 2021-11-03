#include "config.h"

#include <utility>
#include <fstream>
#include <jsoncpp/json/json.h>

namespace orc
{
    class Config::Private
    {
        typedef std::map<std::string, Config::Private*> SubConfigMap;

    public:
        Private( Config* config )
            : config_( config )
            , parent_( nullptr )
        {}

        Private( PrivatePtr parent, const Json::Value &json_value )
            : config_( parent->config_ )
            , parent_( parent )
            , document_root_( json_value )
        {}

        ~Private(void)
        {
            SubConfigMap::iterator iter = sub_config_map_.begin();

            while ( sub_config_map_.end() != iter )
            {
                delete iter->second;
                ++iter;
            }
        }

    public:
        inline bool number( const std::string &key, int32_t &value ) const
        {
            const Json::Value &reference_value = document_root_[key];
            if ( false != reference_value.isInt() )
            {
                value = reference_value.asInt();
                return true;
            }

            return false;
        }

        inline bool number( const std::string &key, uint32_t &value ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isUInt() )
            {
                value = referenced_value.asUInt();
                return true;
            }

            return false;
        }

        inline bool number( const std::string &key, int64_t &value ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isInt64() )
            {
                value = referenced_value.asInt64();
                return true;
            }

            return false;
        }

        inline bool number( const std::string &key, uint64_t &value ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isUInt64() )
            {
                value = referenced_value.asUInt64();
                return true;
            }

            return false;
        }

        inline bool number( const std::string &key, float &value ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isDouble() )
            {
                value = referenced_value.asFloat();
                return true;
            }

            return false;
        }

        inline bool number( const std::string &key, double &value ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isDouble() )
            {
                value = referenced_value.asDouble();
                return true;
            }

            return false;
        }

        inline bool boolean( const std::string &key, bool &value ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isBool() )
            {
                value = referenced_value.asBool();
                return true;
            }

            return false;
        }

        inline bool string( const std::string &key, std::string &value ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isString() )
            {
                value = referenced_value.asString();
                return true;
            }

            return false;
        }

        // not good...
        inline bool stringList( const std::string &key, StringList &string_list ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false != referenced_value.isArray() )
            {
                string_list.clear();

                Json::Value::const_iterator iter = referenced_value.begin();
                while ( referenced_value.end() != iter )
                {
                    string_list.push_back( iter->asString() );
                    ++iter;
                }

                return true;
            }

            return false;
        }

        // not good...
        inline bool keyList( StringList &key_list ) const
        {
            const Json::Value &referenced_value = document_root_;
            if ( false != referenced_value.isObject() )
            {
                key_list.clear();

                Json::Value::const_iterator iter = referenced_value.begin();
                while ( referenced_value.end() != iter )
                {
                    key_list.push_back( iter.key().asString() );
                    ++iter;
                }

                return true;
            }

            return false;
        }

        inline bool objectStream( const std::string &key, std::string &object_stream ) const
        {
            const Json::Value &referenced_value = document_root_[key];
            if ( false == referenced_value.isNull() )
            {
                Json::FastWriter fast_writer;
                object_stream = fast_writer.write( referenced_value );
                return true;
            }

            return false;
        }

        inline Private *subSetPrivate( const std::string &key )
        {
            SubConfigMap::iterator found = sub_config_map_.find( key );

            if ( sub_config_map_.end() != found )
            {
                return found->second;
            }

            Json::Value &referenced_value = document_root_[key];

            if ( false == referenced_value.isNull() && false != referenced_value.isObject() )
            {
                PrivatePtr new_private = new Private( this, referenced_value );
                sub_config_map_.insert( std::make_pair( key, new_private ) );

                return new_private;
            }

            return nullptr;
        }

        inline bool bindSubSetPrivate( SubSet &sub_set )
        {
            PrivatePtr new_private = subSetPrivate( sub_set.key_ );

            if ( nullptr != new_private )
            {
                return sub_set.bind( new_private );
            }

            return false;
        }

        inline bool load( const std::string &pathName )
        {
            std::ifstream json_file( pathName, std::ifstream::binary );

            if ( false == json_file.is_open() )
            {
                return false;
            }

            document_root_.clear();

            json_file >> document_root_;

            return true;
        }

    private:
        Config *config_;
        PrivatePtr parent_;
        Json::Value document_root_;
        SubConfigMap sub_config_map_;
    };

    Config::SubSet::SubSet( const std::string &key )
        : key_( key )
    {
    }

    Config::SubSet::~SubSet( void )
    {
    }

    bool Config::SubSet::bind( Config::PrivatePtr private_ptr )
    {
        private_ = private_ptr;

        if (nullptr != private_)
        {
            return bind();
        }

        return false;
    }

    bool Config::SubSet::number( const std::string &key, int32_t &value ) const
    {
        return private_->number( key, value );
    }

    bool Config::SubSet::number( const std::string &key, uint32_t &value ) const
    {
        return private_->number( key, value );
    }

    bool Config::SubSet::number( const std::string &key, int64_t &value ) const
    {
        return private_->number( key, value );
    }

    bool Config::SubSet::number( const std::string &key, uint64_t &value ) const
    {
        return private_->number( key, value );
    }

    bool Config::SubSet::number( const std::string &key, float &value ) const
    {
        return private_->number( key, value );
    }

    bool Config::SubSet::number( const std::string &key, double &value ) const
    {
        return private_->number( key, value );
    }

    bool Config::SubSet::boolean( const std::string &key, bool &value) const
    {
        return private_->boolean( key, value);
    }

    bool Config::SubSet::string( const std::string &key, std::string &value ) const
    {
        return private_->string( key, value );
    }

    bool Config::SubSet::stringList( const std::string &key, StringList &string_list ) const
    {
        return private_->stringList( key, string_list );
    }

    bool Config::SubSet::keyList( StringList &key_list ) const
    {
        return private_->keyList( key_list );
    }

    bool Config::SubSet::objectStream( const std::string &key, std::string &object_stream ) const
    {
        return private_->objectStream( key, object_stream );
    }

    bool Config::SubSet::bindSubSet( SubSet &sub_set )
    {
        return private_->bindSubSetPrivate( sub_set );
    }

    Config::Config( void )
        : private_( new Private( this ) )
    {
    }

    Config::~Config( void )
    {
    }

    bool Config::bindSubSet( SubSet &sub_set )
    {
        return private_->bindSubSetPrivate( sub_set );
    }

    bool Config::load( const std::string &pathName )
    {
        return private_->load( pathName );
    }

}
