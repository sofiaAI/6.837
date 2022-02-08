// #include "gloo/SceneNode.hpp"
// #include "../IntegratorBase.hpp"
// #include "../IntegratorType.hpp"
// #include "../ParticleSystemBase.hpp"
// #include "../ParticleState.hpp"
// #include "./ClothNode.hpp"
// #include "../IntegratorFactory.hpp"
// #include "gloo/shaders/PhongShader.hpp"
// #include "gloo/components/RenderingComponent.hpp"
// #include "gloo/components/ShadingComponent.hpp"
// #include "gloo/components/MaterialComponent.hpp"
// #include "gloo/debug/PrimitiveFactory.hpp"
// #include "glm/gtx/string_cast.hpp"

// namespace GLOO {
// // implement the constructor
// ClothNode::ClothNode(IntegratorType integrator_type_, std::unique_ptr<ParticleState> state_, float step_size_) {
//   //std::unique_ptr<IntegratorBase<ParticleSystemBase, ParticleState>>
//   //IntegratorFactory to make Integrator
//   integrator = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(integrator_type_);
//   state = * state_;

  
//   step_size = step_size_; // not defined in the .hpp file ...
//   system = make_unique<PendulumSystem>();

//   std::shared_ptr<PhongShader> shader_ = std::make_shared<PhongShader>();
//   std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.2f, 20, 20);

//   for(int i = 0; i < state.positions.size(); i++){
//     auto point = make_unique<SceneNode>();
//     point->CreateComponent<ShadingComponent>(shader_);
//     point->CreateComponent<RenderingComponent>(sphere_mesh_);
//     //point->GetComponentPtr<RenderingComponent>()->SetDrawMode(DrawMode::Lines);
//     point->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
//     points.push_back(point.get());
//     AddChild(std::move(point));

//     // system->AddParticle(5);
//     // if( i != 3){
//     //   system->AddSpring(i, i+1, 15, 0.5);
//     // }
//   }
//   system->FixParticle(0);
//   system->FixParticle(n-1);
// }

// void ClothNode::Update(double delta_time) { // Update
//     double num_steps = delta_time/step_size;
//     if(step_size <= delta_time){
//       double start_time = 0;
//       for(int i = 0; i < num_steps; i++){
//     //      state = integrator->Integrate(*system, state, start_time, delta_time);
//          for(int j = 0; j < state.positions.size(); j ++ ){
//            auto point = points[j];
//            point->GetTransform().SetPosition(state.positions[j]);
//          }
//          start_time += step_size;
//       }
//     }else{
//     //   state = integrator->Integrate(*system, state, 0, delta_time);
//         for(int j = 0; j < state.positions.size(); j ++ ){
//            auto point = points[j];
//            point->GetTransform().SetPosition(state.positions[j]);
//          }
//     }
//     return state;
//     //Make particles in Scene

// }

// }