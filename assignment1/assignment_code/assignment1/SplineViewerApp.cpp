#include "SplineViewerApp.hpp"

#include <fstream>

#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/components/LightComponent.hpp"

#include "CurveNode.hpp"
#include "PatchNode.hpp"
#include <iostream>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

namespace GLOO {

SplineViewerApp::SplineViewerApp(const std::string& app_name,
                                 glm::ivec2 window_size,
                                 const std::string& filename)
    : Application(app_name, window_size), filename_(filename) {
}

void SplineViewerApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  LoadFile(filename_, root);

  auto camera_node = make_unique<ArcBallCameraNode>();
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.7f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.9f, 0.9f, 0.9f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  auto point_light_node = make_unique<SceneNode>();
  point_light_node->CreateComponent<LightComponent>(point_light);
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 4.0f, 5.f));
  root.AddChild(std::move(point_light_node));
}

void SplineViewerApp::LoadFile(const std::string& filename, SceneNode& root) {
  std::fstream fs(GetAssetDir() + filename);
  if (!fs) {
    std::cerr << "ERROR: Unable to open file " + filename + "!" << std::endl;
    return;
  }

  std::string spline_type;
  std::getline(fs, spline_type);

  std::vector<glm::vec3> control_points;
  std::string line;
  for (size_t i = 0; std::getline(fs, line); i++) {
    std::stringstream ss(line);
    float x, y, z;
    ss >> x >> y >> z;
    control_points.push_back(glm::vec3(x, y, z));
  }

  // TODO: set up patch or curve nodes here.
  // The first line of the user-specified file is spline_type, and the specified
  // control points are in control_points, a std::vector of glm::vec3 objects.
  // Depending on the specified spline type, create the appropriate node(s)
  // parameterized by the control points.

  SplineBasis basis; 
  if(spline_type == "Bezier curve"){
    basis = SplineBasis::Bezier;
  }else if(spline_type == "B-Spline curve"){
    basis = SplineBasis::BSpline;
  }


  if(spline_type == "Bezier patch"){
     for(int i = 0; i<= control_points.size() - 16; i += 16){
      std::vector<glm::vec3> patch_sect = {control_points.begin() + i, control_points.begin() + i + 16};
      auto patch = make_unique<PatchNode>(patch_sect, SplineBasis::Bezier);
      root.AddChild(std::move(patch));
    }

  }else{

    //if a bezier or B-spline curve
    //overlap to create curves with 4 control points
    int i = 0; 
    while(control_points.size() - i >= 4){
    std::vector<glm::vec3> control_sub = {control_points.begin() + i, control_points.begin() + i + 4};
    std::unique_ptr<CurveNode> node = make_unique<CurveNode>(control_sub, basis);
    root.AddChild(std::move(node));
    if(spline_type == "Bezier curve"){
      i+=3;
    }else if(spline_type == "B-Spline curve"){
      i++;
    }

  }

  }

}
}  // namespace GLOO
