#include "../ParticleSystemBase.hpp"
#include "../ParticleState.hpp"
#include "../scenes/Spring.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
class PendulumSystem : public ParticleSystemBase {
  public:
  std::vector<float> masses;
  std::vector<Spring> springs;
  float drag_constant;
  std::vector<int> fixed;

  ParticleState ComputeTimeDerivative(const ParticleState& state, float time) const {
    //implement physics of system f(X,t) on each particle
    //gravity force
    Sphere sphere;
    auto new_state = make_unique<ParticleState>();
    std::vector<glm::vec3> forces;
    std::vector<glm::vec3> accelerations;

    //compute gravity and drag
    for(int i = 0; i < state.positions.size(); i++){
        //gravity force (-x)
        float G = masses[i]*9.8;
        glm::vec3 gravity_force = glm::vec3(0,-G,0);



        //viscous drag force
        glm::vec3 drag_force = -drag_constant*state.velocities[i];

        forces.push_back(gravity_force + drag_force);
    }

    //compute each spring force and add to respective particles
    for(int i = 0; i < springs.size(); i++){
        //spring force
        int idx_i = springs[i].particle_indices[0];
        int idx_j = springs[i].particle_indices[1];

        float k = springs[i].stiffness;
        float rl = springs[i].rest_length;

        glm::vec3 d = state.positions[idx_i] - state.positions[idx_j];
        auto d_mag = glm::length(d);

        glm::vec3 spring_force;
        if(d_mag == 0){
            spring_force = glm::vec3(0,0,0);
        }else{
            spring_force = -k*(d_mag - rl)*(d/d_mag);
        }

        forces[idx_i] += spring_force;
        forces[idx_j] -= spring_force;
    }

    //remove forces from fixed particles
    for(int i = 0; i < fixed.size(); i++){
        int idx = fixed[i];
        forces[idx] = glm::vec3(0,0,0);
    }

    //compute acceleration
    for(int i = 0; i < forces.size(); i++){
        accelerations.push_back(forces[i]/masses[i]);
    }
    new_state->positions = state.velocities;
    new_state->velocities = accelerations;

    return *new_state;

    return state;
  }

  void AddParticle(double mass) {
      masses.push_back(mass);
  }

  void FixParticle(int i) {
      fixed.push_back(i);
  }

  void AddSpring(int i1, int i2, double k_, double length_) {
      //add a spring between the particles at indices i1 and i2 with given spring constant and rest length
      auto new_spring = make_unique<Spring>();
      new_spring->particle_indices = {i1, i2};
      new_spring->stiffness = k_;
      new_spring->rest_length = length_;
      springs.push_back(*new_spring);
  }

  void SetDragConstant(float d) {
      drag_constant = d;
  }


};
}  // namespace GLOO
