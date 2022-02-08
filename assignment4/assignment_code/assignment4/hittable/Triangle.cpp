#include "Triangle.hpp"

#include <iostream>
#include <stdexcept>

#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Plane.hpp"

namespace GLOO {
Triangle::Triangle(const glm::vec3& p0,
                   const glm::vec3& p1,
                   const glm::vec3& p2,
                   const glm::vec3& n0,
                   const glm::vec3& n1,
                   const glm::vec3& n2) {
    positions_.push_back(p0);
    positions_.push_back(p1);
    positions_.push_back(p2);
    normals_.push_back(n0);
    normals_.push_back(n1);
    normals_.push_back(n2);
}

Triangle::Triangle(const std::vector<glm::vec3>& positions,
                   const std::vector<glm::vec3>& normals) {
    positions_ = positions;
    normals_ = normals;
}

bool Triangle::Intersect(const Ray& ray, float t_min, HitRecord& record) const {
    glm::mat3 A(GetPosition(0).x - GetPosition(1).x, GetPosition(0).x - GetPosition(2).x, ray.GetDirection().x,
                GetPosition(0).y - GetPosition(1).y, GetPosition(0).y - GetPosition(2).y, ray.GetDirection().y,
                GetPosition(0).z - GetPosition(1).z, GetPosition(0).z - GetPosition(2).z, ray.GetDirection().z);

		glm::vec3 b = GetPosition(0) - ray.GetOrigin();	
    glm::vec3 x = b*glm::inverse(A);
    float beta = x.x;
    float gamma = x.y;
    float t = x.z;
  

    if (beta + gamma > 1){
	    return false;
	  }  

	  if (beta < 0){
	    return false;
	  } 
    // std::cout << "3" <<std::endl;
	  if (gamma < 0){
	    return false;
	  } 
    // std::cout << "4" <<std::endl;

		if (t >= t_min && t < record.time){
      //std::cout << "collisions" <<std::endl;
			record.time = t;
      record.normal = glm::normalize((1.0f - beta - gamma)*GetNormal(0) + beta*GetNormal(1) + gamma*GetNormal(2));
			// if (hit.getMaterial()->validTexture()){
			// 	hit.setTexCoord((1 - beta - gamma)*texCoords[0] + beta * texCoords[1] + gamma * texCoords[2]);
			// }
			return true;
		}	
		return false;
}
}  // namespace GLOO
