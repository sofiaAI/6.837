#include "gloo/SceneNode.hpp"
#include "../IntegratorBase.hpp"
#include "../IntegratorType.hpp"
#include "../ParticleSystemBase.hpp"
#include "../ParticleState.hpp"
#include "./CircleNode.hpp"
#include "../IntegratorFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"

namespace GLOO {
  // implement the constructor
  CircleNode::CircleNode(IntegratorType integrator_type_, std::unique_ptr<ParticleState> state_, float step_size_) {

  //std::unique_ptr<IntegratorBase<ParticleSystemBase, ParticleState>>
  //IntegratorFactory to make Integrator
  integrator = IntegratorFactory::CreateIntegrator<CircularMotionParticleSystem, ParticleState>(integrator_type_);
  state = * state_;
  std::vector<glm::vec3> positions_ = {glm::vec3(-3,-3,0)};
  std::vector<glm::vec3> velocities_ = {glm::vec3(0,0,0)};
  state.positions = positions_;
  state.velocities = velocities_;
  
  step_size = step_size_; // not defined in the .hpp file ...
  system = make_unique<CircularMotionParticleSystem>();

  std::shared_ptr<PhongShader> shader_ = std::make_shared<PhongShader>();
  std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.5f, 20, 20);
  CreateComponent<ShadingComponent>(shader_);
  CreateComponent<RenderingComponent>(sphere_mesh_);
  CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
}

void CircleNode::Update(double delta_time) { // Update
    double num_steps = delta_time/step_size;
    if(step_size <= delta_time){
      double start_time = 0;
      for(int i = 0; i < num_steps; i++){
         state = integrator->Integrate(*system, state, start_time, delta_time);
         GetTransform().SetPosition(state.positions[0]);
         start_time += step_size;
      }
    }else{
      state = integrator->Integrate(*system, state, 0, delta_time);
      GetTransform().SetPosition(state.positions[0]);
    }
    //Make particles in Scene

}

}