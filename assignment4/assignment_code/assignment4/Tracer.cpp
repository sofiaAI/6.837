#include "Tracer.hpp"

#include <glm/gtx/string_cast.hpp>
#include <stdexcept>
#include <algorithm>

#include "gloo/Transform.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/lights/AmbientLight.hpp"

#include "gloo/Image.hpp"
#include "Illuminator.hpp"
#include "glm/ext.hpp"

namespace GLOO {
void Tracer::Render(const Scene& scene,
                    const std::string& output_file) {
  scene_ptr_ = &scene;

  auto& root = scene_ptr_->GetRootNode();
  tracing_components_ = root.GetComponentPtrsInChildren<TracingComponent>();
  light_components_ = root.GetComponentPtrsInChildren<LightComponent>();

  Image image(image_size_.x, image_size_.y);

  for (size_t y = 0; y < image_size_.y; y++) {
    for (size_t x = 0; x < image_size_.x; x++) {
      // TODO: For each pixel, cast a ray, and update its value in the image.
      // cast ray

      //convert from x,y to pixel coordinates
      //range from -1 to 1, tested :)
      float i = (2*float(y) / (image_size_.y-1)) -1 ;
      float j = (2*float(x) / (image_size_.x-1)) -1 ;

      //coord (horizontal, up)
      glm::vec2 coord = glm::vec2(j, i);
      
      //get perspective camera from hpp to generate ray
      const Ray r = camera_.GenerateRay(coord);
      //initialize a Hit Record
      HitRecord rec;
      // size_t bounces = ?

      glm::vec3 color = TraceRay(r, max_bounces_, rec);
      image.SetPixel(x, y, color);
    }
  }

  if (output_file.size())
    image.SavePNG(output_file);
}


glm::vec3 Tracer::TraceRay(const Ray& ray,
                           size_t bounces,
                           HitRecord& record) const {
  // TODO: Compute the color for the cast ray.
  //Get for each tracing component

  int closest_index = -1; 
  //iterate through each tracing component to find the closest point of intersection for the ray
  for (int i = 0; i< tracing_components_.size(); i++){
    //get tranform
    glm::mat4 transform = tracing_components_[i]->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
    Ray ray_copy = ray;
    //make copy of our ray and move the ray from world space into object space.
    ray_copy.ApplyTransform(glm::inverse(transform));
    bool intersect = tracing_components_[i]->GetHittable().Intersect(ray_copy, camera_.GetTMin(), record);

    if(intersect){
        closest_index = i;
        glm::vec4 mod_norm = glm::transpose(glm::inverse(transform)) * glm::vec4(record.normal, 0.f);
        glm::vec3 new_norm = glm::vec3(mod_norm.x, mod_norm.y, mod_norm.z);
        new_norm = glm::normalize(new_norm);
        record.normal = new_norm;
        
    }
  }
    if(closest_index == -1){
      //if no intersect, return background color
      return GetBackgroundColor(ray.GetDirection());
    }else{
      //calculate color value 
      //initialize 
      glm::vec3 diffuse_material = tracing_components_[closest_index]->GetNodePtr()->GetComponentPtr<MaterialComponent>()->GetMaterial().GetDiffuseColor();
      glm::vec3 specular_material = tracing_components_[closest_index]->GetNodePtr()->GetComponentPtr<MaterialComponent>()->GetMaterial().GetSpecularColor();
      float shininess = tracing_components_[closest_index]->GetNodePtr()->GetComponentPtr<MaterialComponent>()->GetMaterial().GetShininess();

      glm::vec3 color = glm::vec3(0,0,0);

      //initialize variables for illuminator
      const glm::vec3 hit_pos = ray.At(record.time);
      glm::vec3 dir_to_light;
      glm::vec3 intensity;
      float dist_to_light;

      glm::vec3 reflectedEyeRay = ray.GetDirection() - (2 * glm::dot(ray.GetDirection(), record.normal) * record.normal);


      //for each light add its contribution to color
      for(int i = 0; i < light_components_.size(); i++ ){
        glm::vec3 ambient_component = glm::vec3(0,0,0);
        glm::vec3 diffuse_component = glm::vec3(0,0,0);
        glm::vec3 specular_component = glm::vec3(0,0,0);

        if(light_components_[i]->GetLightPtr()->GetType() == LightType::Ambient){
          //add ambient term
          glm::vec3 ambient_light = static_cast<AmbientLight*>(light_components_[i]->GetLightPtr())->GetAmbientColor();
         // Iambient = L_ambient · k_diffuse
          ambient_component = ambient_light*diffuse_material;

          
        }else{

          Illuminator::GetIllumination(*light_components_[i], hit_pos, dir_to_light, intensity, dist_to_light);

          //add diffuse and specular term 
          //Idiffuse =clamp(direction,normal)·intensity_diffuse·kdiffuse
          //implement clamp 
          if(glm::dot(dir_to_light, record.normal) > 0 ){
            diffuse_component = glm::dot(dir_to_light, record.normal)*intensity*diffuse_material;
          }

          //Ispecular = clamp(L, R)^s ·L·kspecular
          //clamp
          if(glm::dot(dir_to_light, reflectedEyeRay) > 0 ){
            auto val = pow(glm::dot(dir_to_light, reflectedEyeRay), shininess);
            specular_component = val*intensity*specular_material;
          }else{
            if( shininess == 0){
              specular_component = intensity*specular_material;
            }
          }

          //draw shadows if enabled
          if( Tracer::shadows_enabled_){
            glm::vec3 surface_point = hit_pos + 0.001f*dir_to_light;
            Ray shadow_ray = Ray(surface_point, dir_to_light);
            HitRecord shadow_record;

            for (int i = 0; i< tracing_components_.size(); i++){
                //get tranform
                glm::mat4 transform = tracing_components_[i]->GetNodePtr()->GetTransform().GetLocalToWorldMatrix();
                Ray shadow_ray_copy = shadow_ray;
                //make copy of our ray and move the ray from world space into object space.
                shadow_ray_copy.ApplyTransform(glm::inverse(transform));
                bool intersect = tracing_components_[i]->GetHittable().Intersect(shadow_ray_copy, camera_.GetTMin(), shadow_record);

                if(intersect){
                  if(shadow_record.time < dist_to_light){
                      diffuse_component = glm::vec3(0,0,0);
                      specular_component = glm::vec3(0,0,0);
                  }
                } 
            }
        }


        }
        color += ambient_component + diffuse_component + specular_component;
          
      } 

      //recursion for bounces
      //implement indirect light
      if(bounces > 0){
        //add epsilon term to hit position
        glm::vec3 intersection = hit_pos+ 0.001f*reflectedEyeRay;

        Ray perfect_reflect = Ray(intersection, reflectedEyeRay);
        HitRecord new_record;
        color = color + specular_material*TraceRay(perfect_reflect, bounces - 1,new_record);
      }


      
      return color;
        
		}
  }


glm::vec3 Tracer::GetBackgroundColor(const glm::vec3& direction) const {
  if (cube_map_ != nullptr) {
    return cube_map_->GetTexel(direction);
  } else
    return background_color_;
}
}  // namespace GLOO
