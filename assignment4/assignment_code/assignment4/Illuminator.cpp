#include "Illuminator.hpp"

#include <limits>
#include <stdexcept>

#include <glm/geometric.hpp>

#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/SceneNode.hpp"

namespace GLOO {
void Illuminator::GetIllumination(const LightComponent& light_component,
                                  const glm::vec3& hit_pos,
                                  glm::vec3& dir_to_light,
                                  glm::vec3& intensity,
                                  float& dist_to_light) {
  // Calculation will be done in world space.

  auto light_ptr = light_component.GetLightPtr();
  if (light_ptr->GetType() == LightType::Directional) {

    auto directional_light_ptr = static_cast<DirectionalLight*>(light_ptr);
    dir_to_light = -directional_light_ptr->GetDirection();
    intensity = directional_light_ptr->GetDiffuseColor();
    dist_to_light = std::numeric_limits<float>::max();

  } else {  
    // Implement point light.
    auto point_light_ptr = static_cast<PointLight*>(light_ptr);
    //direction from scene point to light source
    glm::vec3 light_position = light_component.GetNodePtr()->GetTransform().GetPosition();
    dir_to_light = light_position - hit_pos;
    dir_to_light = glm::normalize(dir_to_light);

    //distance from hit point to to light source 
    dist_to_light = glm::distance(hit_pos, light_position);

    //L(xsurf)= I/αd^2
    //I?  light source color?
    glm::vec3 alpha = point_light_ptr->GetAttenuation();
    intensity = point_light_ptr->GetDiffuseColor()/(alpha*dist_to_light*dist_to_light);
    
  }
}
}  // namespace GLOO
