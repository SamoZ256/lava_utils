#ifndef LV_RIGID_BODY_H
#define LV_RIGID_BODY_H

#include "collider.hpp"

namespace lv {

class RigidBodyComponent {
public:
    PhysicsWorld* world;

    glm::vec3 origin = glm::vec3(0.0f);

    float mass = 1.0f;
    float restitution = 1.0f;
    float friction = 1.0f;

    bool syncTransform = true;

    btVector3 intertia;

    btRigidBody* rigidBody = nullptr;

    btDefaultMotionState* motionState;

    RigidBodyComponent(PhysicsWorld& aWorld) : world(&aWorld) { }

    void init(ColliderComponent& collider, glm::vec3 position, glm::vec3 rotation);

    void destroy();

    void setMass();

    void setOrigin();

    void setPosition(glm::vec3 position);

    void getPosition(glm::vec3& position);

    void setRotation(glm::vec3 rotation);

    void getRotation(glm::vec3& rotation);
};

} //namespace lv

#endif
