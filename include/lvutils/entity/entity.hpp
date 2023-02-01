#ifndef LV_ENTITY_H
#define LV_ENTITY_H

#include <entt/entity/registry.hpp>

#define TRANSFORM_COMPONENT_NAME "Transform"
#define MESH_COMPONENT_NAME "Mesh"
#define MATERIAL_COMPONENT_NAME "Material"
#define SCRIPT_COMPONENT_NAME "Script"
#define CAMERA_COMPONENT_NAME "Camera"
#define SPHERE_COLLIDER_COMPONENT_NAME "Sphere collider"
#define BOX_COLLIDER_COMPONENT_NAME "Box collider"
#define RIGID_BODY_COMPONENT_NAME "Rigid body"

namespace lv {

class Entity {
public:
    entt::entity ID;

    static constexpr entt::entity nullEntity = (entt::entity)std::numeric_limits<uint32_t>::max();

    //Scene& scene;
    entt::registry* registry;
    void* window;

    Entity(entt::entity aID, entt::registry* aRegistry) : ID(aID), registry(aRegistry) {}

    template <typename T, typename... Args> T& addComponent(Args &&...args) {
        if (!hasComponent<T>())
        return registry->emplace<T>(ID, std::forward<Args>(args)...);
        return getComponent<T>();
    }

    template <typename T> void removeComponent() {
        registry->remove<T>(ID);
    }

    template <typename T> bool hasComponent() {
        return registry->all_of<T>(ID);
    }

    template <typename T> T& getComponent() {
        return registry->get<T>(ID);
    }

    bool isValid() { return registry->valid(ID); }

    void destroy() { registry->destroy(ID); }

    //Scripting API
    virtual void start() {}

    virtual void update(float dt) {}
};

} //namespace lv

#endif
