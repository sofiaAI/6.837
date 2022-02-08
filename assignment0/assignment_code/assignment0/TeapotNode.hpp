#ifndef TEAPOT_NODE_H_ // This is the header guard 
#define TEAPOT_NODE_H_
#include "gloo/SceneNode.hpp"
namespace GLOO {
class TeapotNode : public SceneNode {
public:
TeapotNode();
void Update(double delta_time) override;
}; }
#endif