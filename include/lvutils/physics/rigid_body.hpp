#ifndef LV_RIGID_BODY_H
#define LV_RIGID_BODY_H

#include "collider.hpp"

namespace lv {

class RigidBodyComponent {
public:
    PhysicsWorld* world;

    float mass = 1.0f;

    btRigidBody* rigidBody;

    btDefaultMotionState* motionState;

    RigidBodyComponent(PhysicsWorld& aWorld) : world(&aWorld) { }

    void init(ColliderComponent& collider, glm::vec3& position, glm::vec3& rotation);

    void destroy();
};

} //namespace lv

#endif
