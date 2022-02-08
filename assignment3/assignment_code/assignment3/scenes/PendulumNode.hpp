#include "gloo/SceneNode.hpp"
#include "../IntegratorBase.hpp"
#include "../IntegratorType.hpp"
#include "../ParticleSystemBase.hpp"
#include "../ParticleState.hpp"
#include "../systems/PendulumSystem.hpp"

namespace GLOO {
class PendulumNode : public SceneNode {
  public:
    // define the constructor
    PendulumNode(IntegratorType integrator_type_, std::unique_ptr<ParticleState> state_,
                  std::unique_ptr<PendulumSystem> system_, float step_size_, bool cloth_);
    void Update(double delta_time) override;

    void CreateCloth();

    void CreatePendulum();

    void DrawLines();

    int IndexOf(int i, int j);
  private: 
  std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator;
  ParticleState initial_state;
  ParticleState state;
  PendulumSystem system; 
  bool cloth;

  float step_size;

   std::vector<SceneNode*> points;
   
};
}