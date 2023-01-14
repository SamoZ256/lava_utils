#include "lvutils/physics/rigid_body.hpp"

namespace lv {

void RigidBodyComponent::init(ColliderComponent& collider, glm::vec3& position, glm::vec3& rotation) {
    glm::quat rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    //TODO: adjust this to use the real position and rotation
    motionState = new btDefaultMotionState(btTransform(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(0, 10, 0)));

    btVector3 intertia(0, 0, 0);
    collider.shape->calculateLocalInertia(mass, intertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, motionState, collider.shape, intertia);
    rigidBody = new btRigidBody(sphereRigidBodyCI);

    world->world->addRigidBody(rigidBody);
}

void RigidBodyComponent::destroy() {
    world->world->removeRigidBody(rigidBody);
    delete motionState;
    delete rigidBody;
}

} //namespace lv
