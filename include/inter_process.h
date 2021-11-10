#ifndef _INTER_PROCESS_H_
#define _INTER_PROCESS_H_

class InterProcess : public Singletone<InterProcess>
{
    class Private;
    friend calss InterProcess::Private;
    typedef Private* PrivatePtr;

    friend class Singletone<InterProcess>;

private:
    InterProcess( void );

public:
    ~InterProcess( void );

public:
    void start( void );

private:
    void run( void );

private:
    PrivatePtr private_;
}

#endif // _INTER_PROCESS_H_