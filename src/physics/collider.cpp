#include "lvutils/physics/collider.hpp"

namespace lv {

void ShpereColliderComponent::init() {
    shape = new btSphereShape(radius);
}

} //namespace lv
