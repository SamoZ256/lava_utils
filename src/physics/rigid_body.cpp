#include "lvutils/physics/rigid_body.hpp"

namespace lv {

void RigidBodyComponent::init(ColliderComponent& collider, glm::vec3 position, glm::vec3 rotation) {
    btQuaternion quat;
    quat.setEulerZYX(glm::radians(rotation.z), glm::radians(rotation.y), glm::radians(rotation.x));

    motionState = new btDefaultMotionState(btTransform(quat, btVector3(position.x, position.y, position.z)));
    //btTransform transform;
    //motionState->getWorldTransform(transform);
    //transform->setFromOpenGLMatrix()

    collider.shape->calculateLocalInertia(mass, intertia);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, collider.shape, intertia);
    rigidBodyCI.m_restitution = restitution;
    rigidBodyCI.m_friction = friction;
    rigidBody = new btRigidBody(rigidBodyCI);

    /*
    btTransform transform;
    motionState->getWorldTransform(transform);
    transform.getBasis().setEulerZYX(glm::radians(rotation.z), glm::radians(rotation.y), glm::radians(rotation.x));
    */

    world->world->addRigidBody(rigidBody);
}

void RigidBodyComponent::destroy() {
    world->world->removeRigidBody(rigidBody);
    delete motionState;
    delete rigidBody;
    rigidBody = nullptr;
}

void RigidBodyComponent::setMass() {
    //float crntMass;
    //rigidBody->getMassProps(&crntMass, &intertia);
    //if (crntMass != mass) {
        rigidBody->setMassProps(mass, intertia);
    //}
}

void RigidBodyComponent::setOrigin() {
    btTransform transform;
    motionState->getWorldTransform(transform);
    transform.setOrigin(btVector3(origin.x, origin.y, origin.z));
}

void RigidBodyComponent::setRotation(glm::vec3 rotation) {
    btTransform transform;
    motionState->getWorldTransform(transform);
    transform.getBasis().setEulerZYX(glm::radians(rotation.z), glm::radians(rotation.y), glm::radians(rotation.x));
}

void RigidBodyComponent::getRotation(glm::vec3& rotation) {
    btTransform transform;
    motionState->getWorldTransform(transform);
    transform.getBasis().getEulerZYX(rotation.z, rotation.y, rotation.x);
    rotation = glm::degrees(rotation);
}

} //namespace lv
