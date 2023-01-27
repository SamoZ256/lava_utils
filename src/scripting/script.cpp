#include "lvutils/scripting/script.hpp"

#include <filesystem>
#include <iostream>

#include "lvcore/core/swap_chain.hpp"

#include "lvutils/scripting/script_manager.hpp"

namespace lv {

void Script::init(const char* aFilename) {
    filename = std::string(aFilename);
    scriptName = std::filesystem::path(filename).stem();
    libFilename = g_scriptManager->scriptDir + "/compiled/" + scriptName + ".dylib";
    cmd = "g++ -std=c++20 " + filename + " -o " + libFilename + " -Wno-deprecated-volatile -fPIC -shared -I src -I /Users/samuliak/Documents/lava_utils/include/lvutils/entity -I external/entt/include -I /opt/homebrew/Cellar/glm/0.9.9.8/include -I /Users/samuliak/Documents/lvnd/include -L /Users/samuliak/Documents/lvnd/lib/nobackend -llvnd";
    
    loadFromFile();
}

void Script::loadFromFile() {
    system(cmd.c_str());

    handle = dlopen(libFilename.c_str(), RTLD_NOW | RTLD_GLOBAL);

    entityConstructor = (newEntityFun)dlsym(handle, "newEntity");
}

void Script::refresh() {
    destroy();
    loadFromFile();

    for (ScriptComponent* ref : refs) {
        ref->entity = entityConstructor(ref->entityID, ref->registry);
        ref->entity->window = g_swapChain->_window;
    }
}

void ScriptComponent::destroy() {
    if (isValid()) {
        for (uint16_t i = 0; i < script->refs.size(); i++) {
            if (script->refs[i] == this) {
                script->refs.erase(script->refs.begin() + i);
                break;
            }
        }
        if (script->refs.size() == 0) {
            script->destroy();
        }
    }
}

void ScriptComponent::loadScript(const char* filename, entt::entity aEntityID, entt::registry* aRegistry) {
    entityID = aEntityID;
    registry = aRegistry;
    script = g_scriptManager->loadScript(filename);
    if (script != nullptr) {
        script->refs.push_back(this);
        entity = script->entityConstructor(entityID, registry);
        entity->window = g_swapChain->_window;
    }
}

bool ScriptComponent::isValid() {
    if (script == nullptr || entity == nullptr)
        return false;

    return script->isValid();
}

}
