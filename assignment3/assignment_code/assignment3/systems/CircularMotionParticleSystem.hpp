#include "../ParticleSystemBase.hpp"

namespace GLOO {
class CircularMotionParticleSystem : public ParticleSystemBase {
  public:
  ParticleState ComputeTimeDerivative(const ParticleState& state, float time) const {
    //implement physics of system f(X,t)
    int x = state.positions[0][0];
    int y = state.positions[0][1];

    std::vector<glm::vec3> positions_ = {glm::vec3(-y,x,0)};
    std::vector<glm::vec3> velocities_ = {glm::vec3(0,0,0)};

    auto new_state = make_unique<ParticleState>();
    new_state->positions = positions_;
    new_state->velocities = velocities_;

    return *new_state;
  }
};
}  // namespace GLOO

