#include "MeshViewerApp.hpp"

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
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/cameras/BasicCameraNode.hpp"
#include "TeapotNode.hpp"
#include "LightPositionNode.hpp"

namespace GLOO {
MeshViewerApp::MeshViewerApp(const std::string& app_name,
                             glm::ivec2 window_size)
    : Application(app_name, window_size) {
}

void MeshViewerApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  auto camera_node = make_unique<BasicCameraNode>();
  camera_node->GetTransform().SetPosition(glm::vec3(0.0f, 1.5f, 10.0f));
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.2f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto directional_light = std::make_shared<DirectionalLight>();
  directional_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
  directional_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  directional_light->SetDirection(glm::vec3(1, 1, 0));
  auto directional_light_node = make_unique<SceneNode>();
  directional_light_node->CreateComponent<LightComponent>(directional_light);
  root.AddChild(std::move(directional_light_node));


  auto point_light_node = make_unique<LightPositionNode>();
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 4.0f, 5.f));
  root.AddChild(std::move(point_light_node));

  std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
  std::shared_ptr<VertexObject> mesh =
      MeshLoader::Import("assignment0/teapot.obj").vertex_obj;
  if (mesh == nullptr) {
    return;
  }
  auto teapot = make_unique<TeapotNode>();
  teapot->GetTransform().SetPosition(glm::vec3(0.f, 0.f, 0.f));
  teapot->GetTransform().SetRotation(glm::vec3(1.0f, 0.0f, 0.0f), 0.3f);
  root.AddChild(std::move(teapot));
}
}  // namespace GLOO
