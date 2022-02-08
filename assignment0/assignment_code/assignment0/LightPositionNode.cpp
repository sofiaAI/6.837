#include "LightPositionNode.hpp" // More include here
#include "gloo/InputManager.hpp"
#include "gloo/external.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/components/LightComponent.hpp"
#include <glm/vec3.hpp> // glm::vec3
#define GLFW_KEY_RIGHT 262
#define GLFW_KEY_LEFT 263
#define GLFW_KEY_DOWN 264
#define GLFW_KEY_UP 265

namespace GLOO {
LightPositionNode::LightPositionNode() {
  // Constructor
  auto point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));

  CreateComponent<LightComponent>(point_light);
}
void LightPositionNode::Update(double delta_time) { // Update
    auto current_pos = GetTransform().GetPosition();
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_RIGHT)) {
        GetTransform().SetPosition(current_pos + glm::vec3(0.1f, 0.f, 0.f));
    }
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_LEFT)) {
        GetTransform().SetPosition(current_pos + glm::vec3(-0.1f, 0.f, 0.f));
    }
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_UP)) {
        GetTransform().SetPosition(current_pos + glm::vec3(0.f, 0.1f, 0.f));
    }
    if (InputManager::GetInstance().IsKeyPressed(GLFW_KEY_DOWN)) {
        GetTransform().SetPosition(current_pos + glm::vec3(0.f, -0.1f, 0.f));
    }
} 

}