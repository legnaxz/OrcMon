#ifndef _DIRECTORY_WATCHER_H_
#define _DIRECTORY_WATCHER_H_

#include "common.h"

#include <map>
#include <sys/types.h>

namespace orc {

namespace Actions
{
    enum Action
    {
        Add = 1,
        Delete = 2,
        Modified = 4
    };
};

class DirectoryWatcher;
class DirectoryWatchListener;

typedef Actions::Action Action;

class DirectoryWatcher
{
    class Private;
    friend class DirectoryWatcher::Private;
    typedef Private* PrivatePtr;

   	struct WatchStruct;

public:
    DirectoryWatcher();
    ~DirectoryWatcher();
    unsigned long addWatch( const std::string& dir_path, DirectoryWatchListener* watch_listener, bool recursive );
    void update();

protected:
    void removeWatch( const std::string& dir_path );
    void removeWatch( unsigned long id );
    void handleAction( WatchStruct* watch, const std::string& filename, unsigned long action );

private:
    PrivatePtr private_;

};

class DirectoryWatchListener
{
public:
    DirectoryWatchListener() {}
    virtual ~DirectoryWatchListener() {}
    virtual void handleFileAction( unsigned long id, const std::string& dir_path, const std::string& filenamem, Action action ) = 0;

};

};
#endif