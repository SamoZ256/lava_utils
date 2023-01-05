#ifndef LV_SCRIPT_H
#define LV_SCRIPT_H

#include <dlfcn.h>
#include <string>

#include "../entity/entity.hpp"

namespace lv {

typedef Entity* (*newEntityFun)(entt::entity, entt::registry*);

class ScriptComponent;

class Script {
public:
    void* handle;
    newEntityFun entityConstructor;

    std::string filename;
    std::string scriptName;
    std::string libFilename;
    std::string cmd;
    time_t lastModified;

    std::vector<ScriptComponent*> refs;

    void init(const char* aFilename);

    void destroy() { dlclose(handle); }

    void loadFromFile();

    void refresh();

    bool isValid() { return (handle != nullptr && entityConstructor != nullptr); }
};

class ScriptComponent {
public:
    Script* script = nullptr;
    Entity* entity = nullptr;

    entt::entity entityID;
    entt::registry* registry;

    void destroy();

    void loadScript(const char* filename, entt::entity aEntityID, entt::registry* aRegistry);

    bool isValid();
};

} //namespace lv

#endif
