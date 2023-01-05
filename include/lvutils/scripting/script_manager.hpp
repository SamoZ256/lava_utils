#ifndef LV_SCRIPT_MANAGER_H
#define LV_SCRIPT_MANAGER_H

#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WIN32__
#include <unistd.h>
#endif

#ifdef __WIN32__
#define stat _stat
#endif

#include "script.hpp"

namespace lv {

class ScriptManager {
public:
    std::vector<Script*> scripts;
    std::string scriptDir;

    ScriptManager();

    Script* loadScript(const char* filename);
};

extern ScriptManager* g_scriptManager;

} //namespace lv

#endif
