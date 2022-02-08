#include "TeapotNode.hpp" // More include here
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/InputManager.hpp"
#include <glm/vec3.hpp> // glm::vec3
namespace GLOO {
TeapotNode::TeapotNode() {
  // Constructor
  std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
  std::shared_ptr<VertexObject> mesh =
      MeshLoader::Import("assignment0/teapot.obj").vertex_obj;
  if (mesh == nullptr) {
    return;
  }
  CreateComponent<ShadingComponent>(shader);
  CreateComponent<RenderingComponent>(mesh);
  CreateComponent<MaterialComponent>( std::make_shared<Material>(Material::GetDefault()));
}
void TeapotNode::Update(double delta_time) { // Update
    static bool prev_released = true;
    static int counter = 0;
    if (InputManager::GetInstance().IsKeyPressed('C')) {
    if (prev_released) {
        if (counter == 1){
            GetComponentPtr<MaterialComponent>()->SetMaterial(std::make_shared<Material>(Material::GetDefault()));
            counter = 0;
        }else{
            GetComponentPtr<MaterialComponent>()->GetMaterial().SetAmbientColor(glm::vec3(1,1,1));
            GetComponentPtr<MaterialComponent>()->GetMaterial().SetDiffuseColor(glm::vec3(1,1,1));
            counter = 1;
        }

        
    }
    prev_released = false;
    } else if (InputManager::GetInstance().IsKeyReleased('C')) {
    prev_released = true; } 
} 

}