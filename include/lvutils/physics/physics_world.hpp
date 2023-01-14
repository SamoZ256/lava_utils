#ifndef LV_PHYSICS_WORLD_H
#define LV_PHYSICS_WORLD_H

#include <btBulletDynamicsCommon.h>

namespace lv {

class PhysicsWorld {
public:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* world;

    void init();

    void destroy();
};

} //namespace lv

#endif
