#include "lvutils/physics/collider.hpp"

namespace lv {

void SphereColliderComponent::init() {
    shape = new btSphereShape(radius);
}

void SphereColliderComponent::setRadius() {
    ((btSphereShape*)shape)->getRadius();
}

void BoxColliderComponent::init() {
    shape = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));
}

} //namespace lv
