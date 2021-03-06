#ifndef SKELETON_NODE_H_
#define SKELETON_NODE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/shaders/PhongShader.hpp"

#include <string>
#include <vector>

namespace GLOO {
class SkeletonNode : public SceneNode {
 public:
  enum class DrawMode { Skeleton, SSD };
  struct EulerAngle {
    float rx, ry, rz;
  };

  SkeletonNode(const std::string& filename);
  void LinkRotationControl(const std::vector<EulerAngle*>& angles);
  void Update(double delta_time) override;
  void OnJointChanged();

  SceneNode* ssd_node_;
  std::shared_ptr<PhongShader> shader_;
  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<VertexObject> cylinder_mesh_;

  std::shared_ptr<VertexObject> skin;
  std::vector<glm::vec3> joint_translations_;
  std::vector<std::vector<float>> weight_ptrs;
  std::vector<glm::mat4> B_;
  std::vector<glm::mat4> T_;

  std::vector<SceneNode*> joint_node_ptrs;
  std::vector<SceneNode*> sphere_node_ptrs_;
  std::vector<SceneNode*> bones_;

  std::vector<glm::vec3> positions_;
  std::vector<glm::vec3> normals_;

 private:
  void LoadAllFiles(const std::string& prefix);
  void LoadSkeletonFile(const std::string& path);
  void LoadMeshFile(const std::string& filename);
  void LoadAttachmentWeights(const std::string& path);

  void ToggleDrawMode();
  void DecorateTree();
  void UpdateTree();

  DrawMode draw_mode_;
  // Euler angles of the UI sliders.
  std::vector<EulerAngle*> linked_angles_;
};
}  // namespace GLOO

#endif
