#ifndef _ORC_MANAGER_H_
#define _ORC_MANAGER_H_

#include "common.h"
#include "singletone.h"
#include "config.h"

namespace orc{

    class Manager : public Singleton<Manager>
    {
        class Private;
        friend class Manager::Private;
        typedef Private* PrivatePtr;

        friend class Singleton<Manager>;

    public:
        class OrcConfigFileInfo : public Config::SubSet {
            friend class Manager;

        private:
            OrcConfigFileInfo( void );

        public:
            const std::string& version( void ) const {
                return version_;
            }

        private:
            bool bind( void ) override;

        private:
            std::string version_;    
        };

        class OrcConfig : public Config::SubSet {
            friend class Manager;

        public:
            // class CallLog : public Config::SubSet {
            //     friend class OrcConfig;
            
            // private:
            //     CallLog ( void );
            
            // public:
            //     const std::string& basePath( void ) const noexcept {
            //         return base_path_;
            //     }

            //     bool loggingRawAudio( void ) const noexcept {
            //         return logging_raw_audio_;
            //     }
            
            // private:
            //     bool bind( void ) override;

            // private:
            //     std::string& base_path_;
            //     bool logging_raw_audio_;    
            // };
        
        private:
            OrcConfig( void );
        
        public :   
            // more classes

        private:
            bool bind ( void ) override;

        private:
            //uint32_t max_channels_;
            //CallLog call_log_;
            
        };

    private:
        Manager ( void );

    public:
        ~Manager( void );
    
    public: 
        bool initialize( const std::string& configFilePath );
        
        const OrcConfigFileInfo& orcConfigFileInfo( void ) {
            return orc_config_file_info_;
        }

        const OrcConfig& orcConfig( void ) {
            return orc_config_;
        }

    public:
        void waitForever( void );    

    private:
        PrivatePtr private_;

        OrcConfigFileInfo orc_config_file_info_;
        OrcConfig orc_config_;
    };

}

#endif