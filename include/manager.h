#ifndef _ORC_MANAGER_H_
#define _ORC_MANAGER_H_

#include "common.h"
#include "singletone.h"
#include "config.h"

#include "directorywatcher.h"
#include "codecs.h"

#include <sys/types.h>
#include <sys/stat.h>

namespace orc{

    class UpdateListener;

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
            class Watcher : public Config::SubSet {
                friend class OrcConfig;

            private:
                Watcher( void );
            
            public: 
                const std::string base_path( void ) const noexcept 
                {
                    return base_path_;
                }

                const std::string export_path( void ) const noexcept 
                {
                    return export_path_;
                }
            
            private:
                bool bind( void ) override;

            private:
                std::string base_path_;
                std::string export_path_;
            };

            class InterProcess : public Config::SubSet {
                friend class OrcConfig;

            private: 
                InterProcess( void );
            
            public: 
                const std::string mode( void ) const noexcept
                {
                    return mode_;
                }

                const std::string ip( void ) const noexcept
                {
                    return ip_;
                }

                uint16_t port( void )
                {
                    return port_;
                }
            
            public:
                bool bind( void ) override;

            private:
                std::string mode_;
                std::string ip_;
                uint16_t port_;
            };

            class ProcessFD : public Config::SubSet {
                friend class OrcConfig;
            
            private:
                ProcessFD( void );
            
            public:
                uint16_t max_child( void )
                {
                    return max_childs_;
                }

            public:
                bool bind ( void ) override;
            
            private:
                uint16_t max_childs_;

            };
        
        private:
            OrcConfig( void );
        
        public :   
            const Watcher& watcher( void ) const noexcept {
                return watcher_;
            }

            const InterProcess& interprocess( void ) const noexcept {
                return inter_process_;
            }

            const ProcessFD& processfd( void ) const noexcept {
                return process_fd_;
            }

        public:
            bool bind ( void ) override;

        private:
            Watcher watcher_;
            InterProcess inter_process_;
            ProcessFD process_fd_;
        };

    private:
        Manager ( void );

    public:
        ~Manager( void );
    
    public: 
        bool initialize( const std::string& configFilePath );

        std::string getBaseDir();
        
        const OrcConfigFileInfo& orcConfigFileInfo( void ) {
            return orc_config_file_info_;
        }

        const OrcConfig& orcConfig( void ) {
            return orc_config_;
        }

    public:
        void waitForever( void );
        inline bool isExist( const std::string path )
        {
            if ( stat( path.c_str(), &info_ ) != 0 )
                return false;
            else
                return true; 
        }

    private:
        PrivatePtr private_;

        OrcConfigFileInfo orc_config_file_info_;
        OrcConfig orc_config_;

        unsigned long base_watch_id_;

        struct stat info_;

    };

}

#endif