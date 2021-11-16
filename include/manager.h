#ifndef _ORC_MANAGER_H_
#define _ORC_MANAGER_H_

#include "common.h"
#include "singletone.h"
#include "config.h"

#include "directory_watcher.h"
#include "directory_watch_listener.h"

#include <sys/types.h>
#include <sys/stat.h>

namespace orc{

    class DirectoryWatchListener;

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

                const std::string intermediate_path( void ) const noexcept 
                {
                    return intermediate_path_;
                }
            
            private:
                bool bind( void ) override;

            private:
                std::string base_path_;
                std::string intermediate_path_;
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

                const std::string active_ip( void ) const noexcept
                {
                    return active_ip_;
                }

                uint16_t active_port( void )
                {
                    return active_port_;
                }

                const std::string standby_ip( void ) const noexcept
                {
                    return standby_ip_;
                }

                uint16_t standby_port( void )
                {
                    return standby_port_;
                }
            
            public:
                bool bind( void ) override;

            private:
                std::string mode_;
                std::string active_ip_;
                uint16_t active_port_;
                std::string standby_ip_;
                uint16_t standby_port_;
            };

            class ProcessFD : public Config::SubSet {
                friend class OrcConfig;
            
            private:
                ProcessFD( void );
            
            public:
                const std::string child_ip( void ) const noexcept
                {
                    return child_ip_;
                }

                const uint16_t child_port( void ) const noexcept
                {
                    return child_port_;
                }

            public:
                bool bind ( void ) override;
            
            private:
                std::string child_ip_;
                uint16_t child_port_;

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