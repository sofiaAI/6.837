namespace GLOO {
struct Spring {
  // The state of a spring: connections and spring constant
  std::vector<int> particle_indices;
  double stiffness;
  double rest_length;
};
}
