#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class TrapezoidalIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    // return state at time t + dt using Trapezoidal Rule integration.
    //X(t + dt) = X(t) + dt/2 (f0 + f1)
    //f0 =f(X(t),t)
    //f1 =f(X(t) + dt*f0, t+dt)

    ParticleState f0 = system.ComputeTimeDerivative(state, start_time);
    ParticleState f1 = system.ComputeTimeDerivative(state + dt*f0, start_time + dt);

    TState new_state = state + (dt/2.0) * (f0 + f1);
    return new_state;
  }
};
}  // namespace GLOO