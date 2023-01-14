#include "lvutils/physics/physics_world.hpp"

namespace lv {

void PhysicsWorld::init() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    world->setGravity(btVector3(0, -10, 0));
}

void PhysicsWorld::destroy() {
    delete world;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
}

} //namespace lv
