#include "healthmanager.h"

namespace orc { 
    namespace Health {
        enum Mode {
            StandAlone = 0,
            Active,
            Standby,
            Temporary
        };
    };

    typedef orc::Health::Mode mode;

class HealthManager::Private
{
public:
    Private( HealthManager* health_manager )
        : health_manager_( health_manager )
    {}
    ~Private()
    {}

    inline 
    
private:
    HealthManager* health_manager_;

};

HealthManager::HealthManager()
    : private_( new Private( this ) )
{
}

HealthManager::~HealthManager()
{
    private_->~Private();
    delete private_;
}

}