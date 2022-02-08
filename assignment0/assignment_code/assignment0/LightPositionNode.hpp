#ifndef LIGHT_POSITION_NODE_H_ // This is the header guard 
#define LIGHT_POSITION_NODE_H_
#include "gloo/SceneNode.hpp"

namespace GLOO {
class LightPositionNode : public SceneNode {
public:
LightPositionNode();
void Update(double delta_time) override;
}; }
#endif