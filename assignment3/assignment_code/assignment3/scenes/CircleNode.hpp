#include "gloo/SceneNode.hpp"
#include "../IntegratorBase.hpp"
#include "../ParticleSystemBase.hpp"
#include "../ParticleState.hpp"
#include "../systems/CircularMotionParticleSystem.hpp"

namespace GLOO {
class CircleNode : public SceneNode {
  public:
    // define the constructor
    CircleNode(IntegratorType integrator_type_, std::unique_ptr<ParticleState> state_, float step_size_);
    void Update(double delta_time) override;
  private: 
  std::unique_ptr<IntegratorBase<CircularMotionParticleSystem, ParticleState>> integrator;
  ParticleState state;
  std::unique_ptr<CircularMotionParticleSystem> system;
  float step_size;
};
}