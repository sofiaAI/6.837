#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class RK4Integrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    // return state at time t + dt using RK4

    ParticleState k1 = system.ComputeTimeDerivative(state, start_time);
    ParticleState k2 = system.ComputeTimeDerivative(state + (dt/2.0)*k1, start_time + (dt/2.0));
    ParticleState k3 = system.ComputeTimeDerivative(state + (dt/2.0)*k2, start_time + (dt/2.0));
    ParticleState k4 = system.ComputeTimeDerivative(state + dt*k3, start_time + dt);

    TState new_state = state + (dt/6.0) * (k1 + 2*k2 + 2*k3 + k4);
    return new_state;
  }
};
}  // namespace GLOO