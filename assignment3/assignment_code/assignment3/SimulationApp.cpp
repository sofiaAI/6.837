#include "SimulationApp.hpp"

#include "glm/gtx/string_cast.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/AxisNode.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "integrators/ForwardEulerIntegrator.hpp"
#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "scenes/CircleNode.hpp"


namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size,
                             IntegratorType integrator_type,
                             float integration_step)
    : Application(app_name, window_size),
      integrator_type_(integrator_type),
      integration_step_(integration_step) {
  // TODO: remove the following two lines and use integrator type and step to
  // create integrators; the lines below exist only to suppress compiler
  // warnings.
  // UNUSED(integrator_type_);
  // UNUSED(integration_step_);

}

void SimulationApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  auto camera_node = make_unique<ArcBallCameraNode>(55.f, 45.0f, -100.0f);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  root.AddChild(make_unique<AxisNode>('A'));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(1.0f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  auto point_light_node = make_unique<SceneNode>();
  point_light_node->CreateComponent<LightComponent>(point_light);
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 4.f));
  root.AddChild(std::move(point_light_node));


  //my code starts here
  //states for each simulation
  auto state = make_unique<ParticleState>();

  //simple circle
  auto circle_sim = make_unique<CircleNode>(integrator_type_, std::move(state), integration_step_);
  root.AddChild(std::move(circle_sim));

  //pendulum 
  root.AddChild(SetupPendulum());

  //create cloth nodes and springs
  root.AddChild(SetupCloth());


}

std::unique_ptr<PendulumNode> SimulationApp::SetupPendulum() {
  //Sets up a pendulum with 4 particles 
  auto state_pendulum = make_unique<ParticleState>();
  std::vector<glm::vec3> pendulum_positions_ = {glm::vec3(-7.5,0.5,0), glm::vec3(-4.7,-2.5, 0), glm::vec3(-12.3,-3.5, 0), glm::vec3(-3.6,-6.25,0)};
  std::vector<glm::vec3> pendulum_velocities_ = {glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0,0,0)};
  state_pendulum->positions = pendulum_positions_;
  state_pendulum->velocities = pendulum_velocities_;

  std::unique_ptr<PendulumSystem> system = make_unique<PendulumSystem>();

  auto pendulum_sim = make_unique<PendulumNode>(integrator_type_, std::move(state_pendulum), 
                                                std::move(system), integration_step_, false);
  return pendulum_sim;
}

std::unique_ptr<PendulumNode> SimulationApp::SetupCloth() {
  auto state_cloth = make_unique<ParticleState>();

  //Sets up a cloth made from an 8x8 particle grid
  int n = 8;
  glm::vec3 top_right = glm::vec3(-35, 10,0);
  std::vector<glm::vec3> cloth_positions_ = {};
  std::vector<glm::vec3> cloth_velocities_ = {};
  for(int i = 0; i < n; i ++){
    for(int j = 0; j < n; j ++){
        glm::vec3 new_node = top_right + glm::vec3(3*j, 0, 0) + glm::vec3(0, -i*3, 0);
        cloth_positions_.push_back(new_node);
        cloth_velocities_.push_back(glm::vec3(0,0,0));

    }
  }
  state_cloth->positions = cloth_positions_;
  state_cloth->velocities = cloth_velocities_;

  std::unique_ptr<PendulumSystem> cloth_system = make_unique<PendulumSystem>();
  auto cloth_sim = make_unique<PendulumNode>(integrator_type_, std::move(state_cloth),
                                            std::move(cloth_system), integration_step_, true);

  return cloth_sim;
}

}  // namespace GLOO
