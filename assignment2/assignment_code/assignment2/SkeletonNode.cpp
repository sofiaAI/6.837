#include "SkeletonNode.hpp"

#include <fstream>
#include "glm/gtx/string_cast.hpp"
#include <map>


#include "gloo/utils.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"

namespace GLOO {
SkeletonNode::SkeletonNode(const std::string& filename)
    : SceneNode(), draw_mode_(DrawMode::Skeleton) {

  // Initialize shader and mesh
  shader_ = std::make_shared<PhongShader>();
  sphere_mesh_ = PrimitiveFactory::CreateSphere(0.02f, 25, 25);
  cylinder_mesh_ = PrimitiveFactory::CreateCylinder(0.01f, 1.f, 25);

  LoadAllFiles(filename);


  // Create ssd node
  auto ssd_node_ptr = make_unique<SceneNode>();
  ssd_node_ = ssd_node_ptr.get();
  ssd_node_->SetActive(false);
  AddChild(std::move(ssd_node_ptr));

  // Calculate Bi's and initial Ti
  for (int i = 0; i < joint_node_ptrs.size(); i++) {
    T_.push_back(joint_node_ptrs[i]->GetTransform().GetLocalToAncestorMatrix(this));
    B_.push_back(glm::inverse(joint_node_ptrs[i]->GetTransform().GetLocalToAncestorMatrix(this)));
  }

  // Draw joints and bones
  DecorateTree();

  // Force initial update.
  OnJointChanged();

}

void SkeletonNode::ToggleDrawMode() {
  draw_mode_ =
      draw_mode_ == DrawMode::Skeleton ? DrawMode::SSD : DrawMode::Skeleton;
  // TODO: implement here toggling between skeleton mode and SSD mode.
  // The current mode is draw_mode_;
  // Hint: you may find SceneNode::SetActive convenient here as
  // inactive nodes will not be picked up by the renderer.

  if (draw_mode_ == DrawMode::SSD) {
    ssd_node_->SetActive(true);
    for (int i = 0; i < joint_node_ptrs.size(); i++) {
      joint_node_ptrs[i]->SetActive(false);
      bones_[i]->SetActive(false);
    }
  }else{
    ssd_node_->SetActive(false);
    for (int i = 0; i < joint_node_ptrs.size(); i++) {
      joint_node_ptrs[i]->SetActive(true);
      bones_[i]->SetActive(true);
    }
  }
}

void SkeletonNode::DecorateTree() {
  // Set up addtional nodes, add necessary components here.
  // You should create one set of nodes/components for skeleton mode
  // (spheres for joints and cylinders for bones), and another set for
  // SSD mode (you could just use a single node with a RenderingComponent
  // that is linked to a VertexObject with the mesh information. Then you
  // only need to update the VertexObject - updating vertex positions and
  // recalculating the normals, etc.).

  // The code snippet below shows how to add a sphere node to a joint.
  // Suppose you have created member variables shader_ of type
  // std::shared_ptr<PhongShader>, and sphere_mesh_ of type
  // std::shared_ptr<VertexObject>.
  // Here sphere_nodes_ptrs_ is a std::vector<SceneNode*> that stores the
  // pointer so the sphere nodes can be accessed later to change their
  // positions. joint_ptr is assumed to be one of the joint node you created
  // from LoadSkeletonFile (e.g. you've stored a std::vector<SceneNode*> of
  // joint nodes as a member variable and joint_ptr is one of the elements).
  //
  // auto sphere_node = make_unique<SceneNode>();
  // sphere_node->CreateComponent<ShadingComponent>(shader_);
  // sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
  // sphere_nodes_ptrs_.push_back(sphere_node.get());
  // joint_ptr->AddChild(std::move(sphere_node));

  for (int i = 0; i < joint_node_ptrs.size(); i++) {

    auto joint_ptr = joint_node_ptrs[i];

    // Create sphere node
    auto sphere_node = make_unique<SceneNode>();

    // Add components
    sphere_node->CreateComponent<ShadingComponent>(shader_);
    sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));

    if(i>0){
    auto bone_ptr = bones_[i];
    bone_ptr->CreateComponent<ShadingComponent>(shader_);
    bone_ptr->CreateComponent<RenderingComponent>(cylinder_mesh_);
    bone_ptr->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));

    // Set bone orientation
    glm::vec3 pos = glm::inverse(joint_ptr->GetTransform().GetLocalToParentMatrix()) * glm::vec4(0, 0, 0, 1);
    glm::vec3 up = glm::normalize(joint_ptr->GetTransform().GetWorldUp());
    glm::vec3 axis = glm::normalize(glm::cross(up, pos));
    float cos_theta = glm::dot(up, glm::normalize(pos));
    float theta = glm::acos(cos_theta);
    float height = glm::length(pos);
    bone_ptr->GetTransform().SetScale(glm::vec3(1.f, height, 1.f));
    bone_ptr->GetTransform().SetRotation(axis, theta);
    }
    sphere_node_ptrs_.push_back(sphere_node.get());
    joint_ptr->AddChild(std::move(sphere_node));

  }
}

void SkeletonNode::UpdateTree() {
  for (int i = 1; i < joint_node_ptrs.size(); i++) {
    auto joint_ptr = joint_node_ptrs[i];
    auto bone_ptr = bones_[i];
    // Set bone orientation
    glm::vec3 pos = glm::inverse(joint_ptr->GetTransform().GetLocalToParentMatrix()) * glm::vec4(0, 0, 0, 1);
    glm::vec3 up = glm::normalize(joint_ptr->GetTransform().GetWorldUp());
    glm::vec3 axis = glm::normalize(glm::cross(up, pos));
    float theta = glm::acos(glm::dot(up, glm::normalize(pos)));
    float height = glm::length(pos);
    bone_ptr->GetTransform().SetScale(glm::vec3(1.f, height, 1.f));
    bone_ptr->GetTransform().SetRotation(axis, theta);
  }
}


void SkeletonNode::Update(double delta_time) {
  // Prevent multiple toggle.
  static bool prev_released = true;
  if (InputManager::GetInstance().IsKeyPressed('S')) {
    if (prev_released) {
      ToggleDrawMode();
    }
    prev_released = false;
  } else if (InputManager::GetInstance().IsKeyReleased('S')) {
    prev_released = true;
  }
}

void SkeletonNode::OnJointChanged() {
  // This method is called whenever the values of UI sliders change.
  // The new Euler angles (represented as EulerAngle struct) can be retrieved
  // from linked_angles_ (a std::vector of EulerAngle*).
  // The indices of linked_angles_ align with the order of the joints in .skel
  // files. For instance, *linked_angles_[0] corresponds to the first line of
  // the .skel file.

  // recalculate joint positions
  for (int i = 0; i < linked_angles_.size(); i++) {
    auto euler_angle = linked_angles_[i];
    auto joint = joint_node_ptrs[i];
    glm::quat Q = glm::quat(glm::vec3(euler_angle->rx, euler_angle->ry, euler_angle->rz));
    joint->GetTransform().SetRotation(Q);
  }

  // after recalculating joint positions, update skeleton
  UpdateTree();

  for (int i = 0; i < joint_node_ptrs.size(); i++) {
    T_[i] = (joint_node_ptrs[i]->GetTransform().GetLocalToAncestorMatrix(this));
  }

  // redraw skin mesh
  auto new_positions = make_unique<PositionArray>();
  auto new_normals = make_unique<NormalArray>();

  for (int i = 0; i < skin->GetPositions().size(); i++) {
    glm::vec3 p = positions_[i];
    glm::vec4 p_new = glm::vec4(0);

    glm::vec3 n = normals_[i];
    glm::mat4 mat = glm::mat4(0);

    for (int j = 0; j < joint_node_ptrs.size() - 1; j++) {
      p_new = p_new + weight_ptrs[i][j] * T_[j] * B_[j] * glm::vec4(p[0], p[1], p[2], 1);
      mat = mat + weight_ptrs[i][j] * T_[j] * B_[j];
    }

    glm::vec3 P = glm::vec3(p_new[0], p_new[1], p_new[2]);
    new_positions->push_back(P);

    mat = glm::inverse(glm::transpose(mat));
    glm::vec4 new_n = mat * glm::vec4(n[0], n[1], n[2], 0);;
    glm::vec3 N = glm::vec3(new_n[0], new_n[1], new_n[2]);
    new_normals->push_back(N);
  }

  skin->UpdatePositions(std::move(new_positions));
  skin->UpdateNormals(std::move(new_normals));

  // Add components
  ssd_node_->CreateComponent<ShadingComponent>(shader_);
  ssd_node_->CreateComponent<RenderingComponent>(skin);
  auto material = CreateComponent<MaterialComponent>(std::make_shared<Material>());
  material.GetMaterial().SetAmbientColor(glm::vec3(255, 255, 255));
  material.GetMaterial().SetDiffuseColor(glm::vec3(255, 255, 255));

  ssd_node_->CreateComponent<MaterialComponent>(material);

}

void SkeletonNode::LinkRotationControl(const std::vector<EulerAngle*>& angles) {
  linked_angles_ = angles;
}

void SkeletonNode::LoadSkeletonFile(const std::string& path) {

  // load skeleton file and build the tree of joints.
  std::fstream fs(GetAssetDir() + path);

  std::string line;
  for (int i = 0; std::getline(fs, line); i++) {

    // Parse each line of .skel file
    std::stringstream ss(line);
    float x, y, z, idx;
    ss >> x >> y >> z >> idx;

    // Create bone node
    auto bone_node = make_unique<SceneNode>();
    bones_.push_back(bone_node.get());

    // Create joint_node
    auto joint_node = make_unique<SceneNode>();
    joint_node->GetTransform().SetPosition(glm::vec3(x, y, z));
    joint_node_ptrs.push_back(joint_node.get());
    joint_node->AddChild(std::move(bone_node));

    // Add joint_node as child to its parent
    if (idx == -1) {
      AddChild(std::move(joint_node));
      joint_node.release();
    } else {
      joint_node_ptrs[idx]->AddChild(std::move(joint_node));
    }

  }
}

void SkeletonNode::LoadMeshFile(const std::string& filename) {
  std::shared_ptr<VertexObject> vtx_obj =
      MeshLoader::Import(filename).vertex_obj;

  // Store the bind pose mesh in your preferred way.
  skin = vtx_obj;
  for (int i = 0; i < skin->GetPositions().size(); i++) {
    positions_.push_back(skin->GetPositions().at(i));
  }
  std::map<int, std::vector<glm::vec3>> weighted_norms;
  for (int i = 0; i < skin->GetIndices().size() - 2; i++) {
    int vert1 = skin->GetIndices().at(i);
    int vert2 = skin->GetIndices().at(i + 1);
    int vert3 = skin->GetIndices().at(i + 2);

    glm::vec3 a = skin->GetPositions().at(vert2) - skin->GetPositions().at(vert3);
    glm::vec3 b = skin->GetPositions().at(vert1) - skin->GetPositions().at(vert3);
    glm::vec3 n = glm::cross(a, b);

    weighted_norms[vert1].push_back(n);
    weighted_norms[vert2].push_back(n);
    weighted_norms[vert3].push_back(n);
  }
  auto v_normals = make_unique<NormalArray>();
  for (int i = 0; i < skin->GetIndices().size(); i++) {
    int vertex = skin->GetIndices().at(i);
    std::vector<glm::vec3> adj_norms = weighted_norms[vertex];
    glm::vec3 norm;
    for (int j = 0; j < adj_norms.size(); j++) {
      norm += adj_norms[j];
    }
    v_normals->push_back(glm::normalize(norm));
    normals_.push_back(glm::normalize(norm));
  }
  skin->UpdateNormals(std::move(v_normals));

}

void SkeletonNode::LoadAttachmentWeights(const std::string& path) {
  // load attachment weights.
  std::fstream fs(GetAssetDir() + path);
  std::string line;
  for (int i = 0; std::getline(fs, line); i++) {
    std::stringstream ss(line);
    std::vector<float> weight_vector;
    weight_vector.push_back(0);
    float weight;
    while (ss >> weight) {
      weight_vector.push_back(weight);
    }
    weight_ptrs.push_back(weight_vector);
  }

}

void SkeletonNode::LoadAllFiles(const std::string& prefix) {
  std::string prefix_full = GetAssetDir() + prefix;
  LoadSkeletonFile(prefix_full + ".skel");
  LoadMeshFile(prefix + ".obj");
  LoadAttachmentWeights(prefix_full + ".attach");
}
}  // namespace GLOO
