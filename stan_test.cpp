#include <stan/math.hpp>
#include <iostream>
#include <vector>

using namespace stan::math;
using namespace Eigen;

// How this works is the output of the ODE solver
//  are encased in these special varis
// Calling chain on any one of the varis has the effect
//  of calling chain on all the varis. I think this works
//  cause of how the autodiff Stack works -- all the adjoints
//  for all the varis from the ode solver will be ready by
//  the time we call chain on any of them.
// The boolean finished is used to keep double calling from
//  happening
struct ode_neighbor_vari : public op_v_vari {
  // Each output vari holds a copy of all the info
  //  needed to solve the adjoint eq. as well
  //  as a pointer to all the neighboring varis
  std::vector<ode_neighbor_vari *> neighbors;
  std::vector<VectorXd> yn;
  double dt;
  bool finished;
  
  ode_neighbor_vari(double val, vari* avi) :
    op_v_vari(val, avi) {
    finished = false;
  }
  void chain() {
    if(!finished) {
      int P = neighbors.size();
      int N = yn.size();

      VectorXd l(2);
      l << 0.0, 0.0;

      MatrixXd fyt(2, 2);
      fyt << 0.0, avi_->val_,
        -avi_->val_, 0.0;

      fyt.transposeInPlace();

      MatrixXd fa(2, 2);
      fa << 0.0, 1.0,
        -1.0, 0.0;

      // Eqs from FATODE paper eq. 2.4
      //  r is mu
      //  l is lambda
      double r = 0.0;
      int p = P - 1;
      for(int i = N - 1; i >= 0; i--) {
        // At each time point we produced output, take in a
        //  new adjoint
        if((i + 1) % (N / P) == 0) {
          l[0] += neighbors[p]->adj_;
          p -= 1;
        }
        
        r -= (-fa * yn[i]).dot(l) * dt;

        l -= -fyt * l * dt;
      }

      avi_->adj_ += r;
      
      for(auto it = neighbors.begin(); it != neighbors.end(); it++) {
        (*it)->finished = true;
      }
    }
  }
};

// Run an ODE that just goes in a circle in 2D space
//  The output is 10 evenly spaced samples of the first
//  state (should be a sinusoidal wave)
inline std::vector<var> ode(const var& a_) {
  const int P = 10;
  std::vector<var> out;
  std::vector<ode_neighbor_vari *> neighbors;

  double a = a_.val();

  double dt = 0.001;
  int N = 1000;

  VectorXd y(2);
  y << 1.0, 0.0;

  std::vector<VectorXd> yn(N);
  MatrixXd f(2, 2);
  f << 0.0, a,
    -a, 0.0;

  int m = 0;
  for(int i = 0; i < N; i++) {
    y += f * y * dt;

    yn[i] = y;

    if((i + 1) % (N / P) == 0) {
      //std::cout << "t = " << (i + 1) * dt << ", y = [" << y(0) << ", " << y(1) << "]" << std::endl;
      neighbors.push_back(new ode_neighbor_vari(y[0], a_.vi_));
    }
  }

  // It's super inefficient to store all the state in every output
  //  vary but whatever
  for(int i = 0; i < P; i++) {
    neighbors[i]->neighbors = neighbors;
    neighbors[i]->yn = yn;
    neighbors[i]->dt = dt;
    out.push_back(var(neighbors[i]));
  }

  return out;
}

int main(int argc, char **argv) {
  var a = 1.0;

  var b = sum(ode(a));

  b.grad();
  std::cout << "value: " << b << ", grad:  " << a.adj() << std::endl;
  std::cout << "Expecting close to: 8.18, -3.44" << std::endl;
}
