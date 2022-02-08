// #include "gloo/SceneNode.hpp"
// #include "../IntegratorBase.hpp"
// #include "../IntegratorType.hpp"
// // #include "../ParticleSystemBase.hpp"
// #include "../ParticleState.hpp"
// #include "../systems/PendulumSystem.hpp"

// namespace GLOO {
// class ClothNode : public SceneNode {
//   public:
//     // define the constructor
//     ClothNode(IntegratorType integrator_type_, std::unique_ptr<ParticleState> state_, float step_size_);
//     void Update(double delta_time) override;
//   private: 
//    std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator;
//    ParticleState state;
//    std::unique_ptr<PendulumSystem> system;
//    float curr_time;
//    float step_size;

//    int n;

//    std::vector<SceneNode*> points;
   
// };
// }