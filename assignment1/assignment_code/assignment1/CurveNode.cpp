#include "CurveNode.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/InputManager.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include "gloo/SceneNode.hpp"


namespace GLOO {
CurveNode::CurveNode(std::vector<glm::vec3> control_points, SplineBasis basis_) {
  // TODO: this node should represent a single spline curve.
  // Think carefully about what data defines a curve and how you can
  // render it.

  control_points_ = control_points;
  spline_basis_ = basis_;

  BBezier = glm::mat4(1.0, 0.0, 0.0, 0.0, -3.0, 3.0, 0.0, 0.0, 3.0, -6.0, 3.0, 0.0, -1.0, 3.0, -3.0, 1.0);
  BSpline  = glm::mat4(1.0/6.0, 4.0/6.0, 1.0/6.0, 0.0, -3.0/6.0, 0.0, 3.0/6.0, 0.0, 3.0/6.0, -6.0/6.0,
                         3.0/6.0, 0, -1.0/6.0, 3.0/6.0, -3.0/6.0, 1.0/6.0);

  C = glm::mat4x3(control_points_[0][0], control_points_[0][1], control_points_[0][2], 
                      control_points_[1][0], control_points_[1][1], control_points_[1][2], 
                      control_points_[2][0], control_points_[2][1], control_points_[2][2], 
                      control_points_[3][0], control_points_[3][1], control_points_[3][2]);

  // Initialize the VertexObjects and shaders used to render the control points,
  // the curve, and the tangent line.
  sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
  curve_polyline_ = std::make_shared<VertexObject>();
  tangent_line_ = std::make_shared<VertexObject>();
  shader_ = std::make_shared<PhongShader>();
  polyline_shader_ = std::make_shared<SimpleShader>();

  InitCurve();
  PlotCurve();
  PlotControlPoints();
  PlotTangentLine();
}

void CurveNode::Update(double delta_time) {

  // Prevent multiple toggle.
  static bool prev_released = true;

  if (InputManager::GetInstance().IsKeyPressed('T')) {
    if (prev_released) {
      // TODO: implement toggling spline bases.
      ToggleSplineBasis();
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyPressed('B')) {
    if (prev_released) {
      // TODO: implement converting conrol point geometry from Bezier to
      // B-Spline basis.
      CurveNode::ConvertGeometry(SplineBasis::BSpline);
      PlotControlPoints();
      PlotCurve();
      PlotTangentLine();
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyPressed('Z')) {
    if (prev_released) {
      // TODO: implement converting conrol point geometry from B-Spline to
      // Bezier basis.
      CurveNode::ConvertGeometry(SplineBasis::Bezier);
      PlotControlPoints();
      PlotCurve();
      PlotTangentLine();
    }
    prev_released = false;
  } else {
    prev_released = true;
  }
}

void CurveNode::ToggleSplineBasis() {
  // TODO: implement toggling between Bezier and B-Spline bases.
   if( spline_basis_ == SplineBasis::Bezier){
     spline_basis_ = SplineBasis::BSpline;
     for(int i = 0; i < points.size(); i++){
       points[i]->RemoveComponent<MaterialComponent>();
       auto mat = CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
       mat.GetMaterial().SetAmbientColor(glm::vec3(0,255,0));
       mat.GetMaterial().SetDiffuseColor(glm::vec3(0,255,0));
       points[i]->CreateComponent<MaterialComponent>(mat);
     }
   } else {
     spline_basis_ = SplineBasis::Bezier;
     for(int i = 0; i < points.size(); i++){
       points[i]->RemoveComponent<MaterialComponent>();
       auto mat = CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
       points[i]->CreateComponent<MaterialComponent>(mat);
     }
   }

  PlotCurve();
  PlotControlPoints();
  PlotTangentLine();
}

void CurveNode::ConvertGeometry(SplineBasis toggle_basis) {
  // TODO: implement converting the control points between bases.

  glm::mat4 matInv = (toggle_basis == SplineBasis::Bezier) ? inverse(BBezier) : inverse(BSpline); 
  glm::mat4 mat = (toggle_basis == SplineBasis::Bezier) ? BSpline: BBezier; 

  C = C*mat*matInv;

}

CurvePoint CurveNode::EvalPatch(float t) {
  auto cp = CurvePoint();
  const glm::vec4 M(1.0, t, pow(t,2), pow(t, 3));
  const glm::vec4 dM(0, 1, 2*t, 3*pow(t,2));

  if(spline_basis_ == SplineBasis::Bezier){
    cp.P = C*(BBezier*M);
    cp.T = C*(BBezier*dM);
  }else{
    cp.P = C*(BSpline*M);
    cp.T = C*(BSpline*dM);
  }

  return cp;
}

void CurveNode::InitCurve() {
  for(int i = 0; i < control_points_.size(); i ++) {
    auto node = make_unique<SceneNode>();  
    node->CreateComponent<ShadingComponent>(shader_);
    node->CreateComponent<RenderingComponent>(sphere_mesh_);
    points.push_back(node.get());
    AddChild(std::move(node));
  }
}

void CurveNode::PlotCurve() {
  auto positions = make_unique<PositionArray>();
  auto indices = make_unique<IndexArray>();

  int i = 0;
  while(i<49){
    CurvePoint point_1= EvalPatch(i/50.0);

    positions->push_back(point_1.P);

    indices->push_back(i);
    indices->push_back(i+1);
    i++;
    
  }

  positions->push_back(EvalPatch(1.0).P);

  curve_polyline_->UpdatePositions(std::move(positions));
  curve_polyline_->UpdateIndices(std::move(indices));

  auto shader = std::make_shared<SimpleShader>();

  auto line_node = make_unique<SceneNode>();
  line_node->CreateComponent<ShadingComponent>(shader);

  auto& rc = line_node->CreateComponent<RenderingComponent>(curve_polyline_);
  rc.SetDrawMode(DrawMode::Lines);

  glm::vec3 color(1.f, 1.f, 1.f);
  auto material = std::make_shared<Material>(color, color, color, 0);
  line_node->CreateComponent<MaterialComponent>(material);

  AddChild(std::move(line_node));


}

void CurveNode::PlotControlPoints() {
  // TODO: plot the curve control points.
  //create vertex object for each point

  for(int i = 0; i < C.length() ; i++) {
    auto node_ = points[i];
    node_->GetTransform().SetPosition(C[i]);
  }
}

void CurveNode::PlotTangentLine() {

  CurvePoint tan_point= EvalPatch(0.5);
  glm::vec3 point_1 = tan_point.P - tan_point.T;
  glm::vec3 point_2 = tan_point.P + tan_point.T;
 

  auto positions = make_unique<PositionArray>();
  positions->push_back(point_1);
  positions->push_back(point_2);

  auto indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);

  tangent_line_->UpdatePositions(std::move(positions));
  tangent_line_->UpdateIndices(std::move(indices));

  auto shader = std::make_shared<SimpleShader>();

  auto line_node = make_unique<SceneNode>();
  line_node->CreateComponent<ShadingComponent>(shader);

  auto& rc = line_node->CreateComponent<RenderingComponent>(tangent_line_);
  rc.SetDrawMode(DrawMode::Lines);

  glm::vec3 color(1.f, 1.f, 1.f);
  auto material = std::make_shared<Material>(color, color, color, 0);
  line_node->CreateComponent<MaterialComponent>(material);

  AddChild(std::move(line_node));
}
}  // namespace GLOO
