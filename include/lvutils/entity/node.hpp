#ifndef NODE_H
#define NODE_H

#include "entity.hpp"

namespace lv {

class NodeComponent {
public:
    entt::entity parent = Entity::nullEntity;
    std::vector<entt::entity> childs;
};

} // namespace lv

#endif
