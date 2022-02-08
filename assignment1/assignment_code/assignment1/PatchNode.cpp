#include "PatchNode.hpp"

#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
PatchNode::PatchNode(std::vector<glm::vec3> points, SplineBasis basis) {
  shader_ = std::make_shared<PhongShader>();
  patch_mesh_ = std::make_shared<VertexObject>();
  spline_basis_ = basis;

  // TODO: this node should represent a single tensor product patch.
  // Think carefully about what data defines a patch and how you can
  // render it.

  for (int d = 0; d < 3; d++) {
    std::vector<float> coords;
    for (int i = 0; i < points.size(); i++) {
      float coord = points[i][d];
      coords.push_back(coord);
    }


    glm::mat4 coordMat(
      coords[0], coords[1], coords[2], coords[3],
      coords[4], coords[5], coords[6], coords[7],
      coords[8], coords[9], coords[10], coords[11],
      coords[12], coords[13], coords[14], coords[15]
    );
    Gs_.push_back(coordMat);
  }


  PlotPatch();
}

PatchPoint PatchNode::EvalPatch(float u, float v) {
  float px = GetPosition(u, v, 0);
  float py = GetPosition(u, v, 1);
  float pz = GetPosition(u, v, 2);

  glm::vec3 p = glm::vec3(px, py, pz);
  glm::vec3 n = GetNormal(u, v);

  PatchPoint point;
  point.P = p;
  point.N = n;

  return point;
}

float PatchNode::GetPosition(float u, float v, int d) {
  // evaluates the spline patch at parameter values u and v and dimension d (0 = x, 1 = y, 2 = z)
  const glm::mat4 BBezier(
    1, 0, 0, 0, -3, 3, 0, 0, 3, -6, 3, 0, -1, 3, -3, 1
  );
  const glm::mat4 BBspline (
    1.0/6.0, 4.0/6.0, 1.0/6.0, 0, -3.0/6.0, 0, 3.0/6.0, 0, 3.0/6.0, -6.0/6.0, 3.0/6.0, 0, -1.0/6.0, 3.0/6.0, -3.0/6.0, 1.0/6.0
  );

  glm::vec4 T_u = {1, u, pow(u, 2), pow(u, 3)};  // monomial basis for u
  glm::vec4 T_v = {1, v, pow(v, 2), pow(v, 3)};  // monomial basis for v
  glm::mat4 B = spline_basis_ == SplineBasis::Bezier ? BBezier : BBspline;  // spline basis
  glm::mat4 Gc = Gs_[d];  // G_x/G_y/G_z

  float pos = glm::dot(T_u, glm::transpose(B) * Gc * B * T_v);

  return pos;
}

glm::vec3 PatchNode::GetNormal(float u, float v) {

  glm::mat4 BBezier(1.0, 0.0, 0.0, 0.0, -3.0, 3.0, 0.0, 0.0, 3.0, -6.0, 3.0, 0.0, -1.0, 3.0, -3.0, 1.0);
  glm::mat4 BSpline(1.0/6.0, 4.0/6.0, 1.0/6.0, 0.0, -3.0/6.0, 0.0, 3.0/6.0, 0.0, 3.0/6.0, -6.0/6.0,
                         3.0/6.0, 0, -1.0/6.0, 3.0/6.0, -3.0/6.0, 1.0/6.0);



  glm::vec4 T_u = {1, u, pow(u, 2), pow(u, 3)};  // monomial basis for u
  glm::vec4 T_v = {1, v, pow(v, 2), pow(v, 3)};  // monomial basis for v
  glm::vec4 dTu = {0, 1, 2*pow(u, 1), 3*pow(u, 2)};  // monomial basis for du
  glm::vec4 dTv = {0, 1, 2*pow(v, 1), 3*pow(v, 2)};  // monomial basis for dv
  glm::mat4 B = spline_basis_ == SplineBasis::Bezier ? BBezier : BSpline;  // spline basis

  float u_x = glm::dot(dTu, (glm::transpose(B) * (Gs_[0] * (B * T_v))));
  float u_y = glm::dot(dTu, (glm::transpose(B) * (Gs_[1] * (B * T_v))));
  float u_z = glm::dot(dTu, (glm::transpose(B) * (Gs_[2] * (B * T_v))));
  glm::vec3 dpdu = glm::vec3(u_x, u_y, u_z);


  float v_x = glm::dot(T_u, (glm::transpose(B) * (Gs_[0] * (B * dTv))));
  float v_y = glm::dot(T_u, (glm::transpose(B) * (Gs_[1] * (B * dTv))));
  float v_z = glm::dot(T_u, (glm::transpose(B) * (Gs_[2] * (B * dTv))));
  glm::vec3 dpdv = glm::vec3(v_x, v_y, v_z);

  glm::vec3 n = glm::cross(dpdu, dpdv);
  float magnitude = pow(pow(n[0], 2) + pow(n[1], 2) + pow(n[2], 2), 0.5);
  glm::vec3 N = glm::vec3(n[0]/magnitude, n[1]/magnitude, n[2]/magnitude);

  return N;
}

void PatchNode::PlotPatch() {
  auto positions = make_unique<PositionArray>();
  auto normals = make_unique<NormalArray>();
  auto indices = make_unique<IndexArray>();


  // TODO: fill "positions", "normals", and "indices"
  float step = 1.0 / (float)(N_SUBDIV_ - 1);


  for (int i = 0; i < N_SUBDIV_; i++) {
    for (int j = 0; j < N_SUBDIV_; j++) {
      float curr_u = (float)i * step;
      float curr_v = (float)j * step;


      PatchPoint pt1 = EvalPatch(curr_u, curr_v);
      positions->push_back(pt1.P);
      normals->push_back(pt1.N);
    }
  }

  for (int k = 0; k < N_SUBDIV_; k++) {
    for (int m = 0; m < N_SUBDIV_; m++) {
      if (k < N_SUBDIV_ - 1 && m < N_SUBDIV_ - 1) {
        indices->push_back(FlattenCoords(k+1, m+1));
        indices->push_back(FlattenCoords(k+1, m));
        indices->push_back(FlattenCoords(k, m));

        indices->push_back(FlattenCoords(k+1, m+1));
        indices->push_back(FlattenCoords(k, m));
        indices->push_back(FlattenCoords(k, m+1));
      }
    }
  }

  patch_mesh_->UpdatePositions(std::move(positions));
  patch_mesh_->UpdateNormals(std::move(normals));
  patch_mesh_->UpdateIndices(std::move(indices));

  auto patch = make_unique<SceneNode>();
  patch->CreateComponent<ShadingComponent>(shader_);
  patch->CreateComponent<RenderingComponent>(patch_mesh_);
  patch->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
  AddChild(std::move(patch));

}

int PatchNode::FlattenCoords(int u, int v) {

  return (N_SUBDIV_ * u) + v;
}

}  // namespace GLOO
