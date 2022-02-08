#ifndef CURVE_NODE_H_
#define CURVE_NODE_H_

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"

namespace GLOO {

enum class SplineBasis { Bezier, BSpline };

struct CurvePoint {
  glm::vec3 P;
  glm::vec3 T;
};

class CurveNode : public SceneNode {
 public:
  CurveNode(std::vector<glm::vec3> control_points_, SplineBasis basis_);
  void Update(double delta_time) override;

 private:
  void ToggleSplineBasis();
  void ConvertGeometry(SplineBasis toggle_basis);
  CurvePoint EvalPatch(float t);
  void InitCurve();
  void PlotCurve();
  void PlotControlPoints();
  void PlotTangentLine();

  std::vector<glm::vec3> control_points_;
  SplineBasis spline_basis_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<VertexObject> curve_polyline_;
  std::shared_ptr<VertexObject> tangent_line_;

  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<ShaderProgram> polyline_shader_;

  glm::mat4 BBezier;
  glm::mat4 BSpline;

  glm::mat4x3 C;

  std::vector<SceneNode*> points;

  const int N_SUBDIV_ = 50;
};
}  // namespace GLOO

#endif
