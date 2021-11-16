#ifndef _DIRECTORY_WATCHER_H_
#define _DIRECTORY_WATCHER_H_

#include "common.h"
#include "directory_watch_listener.h"

#include <map>
#include <sys/types.h>

namespace orc {


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
    void start();

protected:
    void removeWatch( const std::string& dir_path );
    void removeWatch( unsigned long id );
    void handleAction( WatchStruct* watch, const std::string& filename, unsigned long action );

private:
    PrivatePtr private_;

};



};
#endif