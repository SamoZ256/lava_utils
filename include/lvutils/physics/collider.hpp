#ifndef LV_COLLISION_SHAPE_H
#define LV_COLLISION_SHAPE_H

#include "lvutils/libraries/glm.hpp"

#include "physics_world.hpp"

namespace lv {

class ColliderComponent {
public:
    btCollisionShape* shape;

    virtual void init() { throw std::runtime_error("Cannot call the 'init' method of base class 'Collider'"); }

    virtual void destroy() { delete shape; }
};

class SphereColliderComponent : public ColliderComponent {
public:
    float radius = 1.0f;

    void init();

    void setRadius();
};

class BoxColliderComponent : public ColliderComponent {
public:
    glm::vec3 scale = glm::vec3(1.0f);

    void init();

    void setScale();
};

} //namespace lv

#endif
