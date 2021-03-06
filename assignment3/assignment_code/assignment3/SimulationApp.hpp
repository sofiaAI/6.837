#ifndef SIMULATION_APP_H_
#define SIMULATION_APP_H_

#include "gloo/Application.hpp"
#include "IntegratorType.hpp"
#include "scenes/PendulumNode.hpp"

namespace GLOO {
class SimulationApp : public Application {
 public:
  SimulationApp(const std::string& app_name,
                glm::ivec2 window_size,
                IntegratorType integrator_type,
                float integration_step);
  void SetupScene() override;
  std::unique_ptr<PendulumNode> SetupPendulum();
  std::unique_ptr<PendulumNode> SetupCloth();
 private:
  IntegratorType integrator_type_;
  float integration_step_;
};
}  // namespace GLOO

#endif
