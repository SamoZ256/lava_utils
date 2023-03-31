#include "lvutils/scripting/script_manager.hpp"

#include <iostream>

namespace lv {

ScriptManager* g_scriptManager = nullptr;

ScriptManager::ScriptManager() { g_scriptManager = this; }

Script* ScriptManager::loadScript(const char* filename) {
    std::string strFilename(filename);
    for (auto& script : scripts) {
        if (script->filename == strFilename) {
            if (script->handle == nullptr)
                script->loadFromFile();
            return script;
        }
    }

    Script* script = new Script;
    script->init(filename);
    if (script->isValid()) {
        scripts.push_back(script);
            
        struct stat result;
        if (stat(script->filename.c_str(), &result) == 0) {
            script->lastModified = result.st_mtime;
        }

        return script;
    }

    return nullptr;
}

} //namespace lv
