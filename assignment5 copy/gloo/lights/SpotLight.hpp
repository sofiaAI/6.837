
#ifndef GLOO_SPOT_LIGHT_H_
#define GLOO_SPOT_LIGHT_H_

#include "LightBase.hpp"

namespace GLOO {
class SpotLight : public LightBase {
 public:
  void SetSpotDirection(const glm::vec3& direction) {
    direction_ = glm::normalize(direction);
  }

  glm::vec3 GetSpotDirection() const {
    return direction_;
  }

  void SetCutoffAngle(const float angle) {
    angle_ = angle;
  }

  float GetCutoffAngle() const {
    return angle_;
  }
  void SetColor(const glm::vec3& color) {
    color_ = color;
  }
  glm::vec3 GetColor() const {
    return color_;
  }


  LightType GetType() const override {
    return LightType::Spot;
  }

 private:
  glm::vec3 direction_;
  float angle_;
  glm::vec3 color_;
};
}  // namespace GLOO

#endif
