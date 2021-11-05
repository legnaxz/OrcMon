#ifndef _HEALTH_MANAGER_H_
#define _HEALTH_MANAGER_H_

namespace orc
{

class HealthManager
{

    class Private;
    friend class HealthManager::Private;
    typedef Private* PrivatePtr;

public:
    HealthManager();
    ~HealthManager();
    
public:
    //virtaul initialize()
    //virtual deinitialize()

class ModeSingleAlone
{
    //initialize();
    //deinitialize();
};

class ModeActive
{
    //WaitWho( enum mode value )
    //initialize();
    //deinitialize();
};

class ModeStandby
{
    //SendCheckAlive( ms )
    //Expired() 
};

class ModeTemporary
{
    //SendCheckAlive( ms );
    //initialize();
    //deinitialize();
};

private:
    PrivatePtr private_;

};

};

#endif //_HEALTH_MANAGER_H_