#include "gloo/SceneNode.hpp"
#include "../IntegratorBase.hpp"
#include "../IntegratorType.hpp"
#include "../ParticleSystemBase.hpp"
#include "../ParticleState.hpp"
#include "./PendulumNode.hpp"
#include "../IntegratorFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/InputManager.hpp"
#include "glm/gtx/string_cast.hpp"
#include "gloo/shaders/SimpleShader.hpp"

namespace GLOO {
// implement the constructor
PendulumNode::PendulumNode(IntegratorType integrator_type_, std::unique_ptr<ParticleState> state_,
                          std::unique_ptr<PendulumSystem> system_, float step_size_,bool cloth_) {
  //std::unique_ptr<IntegratorBase<ParticleSystemBase, ParticleState>>
  //IntegratorFactory to make Integrator
  integrator = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(integrator_type_);
  state = * state_;
  initial_state = * state_;
  cloth = cloth_;
  step_size = step_size_; // not defined in the .hpp file ...
  system = * system_;
  std::shared_ptr<PhongShader> shader_ = std::make_shared<PhongShader>();
  std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.5f, 20, 20);

  for(int i = 0; i < state.positions.size(); i++){
    auto point = make_unique<SceneNode>();
    point->CreateComponent<ShadingComponent>(shader_);
    point->CreateComponent<RenderingComponent>(sphere_mesh_);
    //point->GetComponentPtr<RenderingComponent>()->SetDrawMode(DrawMode::Lines);
    point->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
    points.push_back(point.get());
    AddChild(std::move(point));
    //add particles
    if(!cloth){
      system.AddParticle(5);
    }else{
      system.AddParticle(10);
    }

  }

  //add springs
  if(!cloth){
    system.SetDragConstant(1.5);
    CreatePendulum();
  }else{
    system.SetDragConstant(20);
    CreateCloth();
  }

  //fix particles

  system.FixParticle(0);

}

void PendulumNode::Update(double delta_time) { // Update
    if (InputManager::GetInstance().IsKeyPressed('R')) {
      state = initial_state;
    }

    double num_steps = delta_time/step_size;
    if(step_size <= delta_time){
      double start_time = 0;
      for(int i = 0; i < num_steps; i++){
         state = integrator->Integrate(system, state, start_time, delta_time);
         for(int j = 0; j < state.positions.size(); j ++ ){
           auto point = points[j];
           point->GetTransform().SetPosition(state.positions[j]);
         }
         start_time += step_size;
      }
    }else{
      state = integrator->Integrate(system, state, 0, delta_time);
      for(int j = 0; j < state.positions.size(); j ++ ){
           auto point = points[j];
           point->GetTransform().SetPosition(state.positions[j]);
         }
    }
    //Make particles in Scene

    if(cloth){
      DrawLines();
  }

}

//returns 1d coordinate from 2d
int PendulumNode::IndexOf(int i, int j){
    return j*8 + i;
}

void PendulumNode::CreatePendulum(){
  for(int i = 0; i < state.positions.size()-1; i++){
      system.AddSpring(i, i+1, 15, 0.5);
    }
}

void PendulumNode::CreateCloth(){
  float structure_k = 500;
  float shear_k = 700;
  float flex_k = 50;

  for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        //structural springs
        if(i != 7){
          system.AddSpring(IndexOf(i, j), IndexOf(i+ 1, j), structure_k, 3);

        }
        if(j != 7){
          system.AddSpring(IndexOf(i, j), IndexOf(i, j+1), structure_k, 3);
        }
        //shear springs
        if(i > 0 && j < 7){
          system.AddSpring(IndexOf(i-1, j+1), IndexOf(i, j), shear_k, 4.25);
        }
        if(i < 7 && j < 7){
          system.AddSpring(IndexOf(i, j), IndexOf(i+1, j+1), shear_k, 4.25);
        }

        //flex springs
        if(i < 6){
          system.AddSpring(IndexOf(i, j), IndexOf(i+ 2, j), flex_k, 6);
        }
        if(j < 6){
          system.AddSpring(IndexOf(i, j), IndexOf(i, j+2), flex_k, 6);
        }


      }
    }
  //fix cloth corners
  system.FixParticle(0);
  system.FixParticle(7);
}

void PendulumNode::DrawLines(){
  // std::shared_ptr<VertexObject> segment = std::make_shared<VertexObject>();
  // auto shader = std::make_shared<SimpleShader>();
  // auto line_node = make_unique<SceneNode>();
  // std::unique_ptr<PositionArray> particle_positions = make_unique<PositionArray>();
  // std::unique_ptr<IndexArray> indices = make_unique<IndexArray>();

  // for(int i = 0; i < system.springs.size(); i++){
  //     int i1 = system.springs[i].particle_indices[0];
  //     int i2 = system.springs[i].particle_indices[1];

  //     particle_positions->push_back(state.positions[i1]);
  //     particle_positions->push_back(state.positions[i2]);

  //     indices->push_back(i1);
  //     indices->push_back(i2);

  // }

  // segment->UpdatePositions(std::move(particle_positions));
  // segment->UpdateIndices(std::move(indices));

  // glm::vec3 color(1.f, 1.f, 1.f);
  // auto material = std::make_shared<Material>(color, color, color, 0);
  // line_node->CreateComponent<MaterialComponent>(material);

  // line_node->CreateComponent<ShadingComponent>(shader);
  // auto& rc = line_node->CreateComponent<RenderingComponent>(segment);
  // rc.SetDrawMode(DrawMode::Lines);

  // AddChild(std::move(line_node));

}

}
