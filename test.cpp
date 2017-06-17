//#include <stan/math.hpp>
#include <iostream>
#include <vector>

//using namespace stan::math;

int main(int argc, char **argv) {
  double a = -1.0;
  double b = 1.0;

  double dt = 0.001;
  int N = int(5.0 / dt);
  std::vector<int> print = {10, 20, 40, 100, N};

  std::vector<double> y = {1.0, 0.0};
  std::vector<std::vector<double> > yn;

  std::vector<int>::iterator pit = print.begin();
  for(int i = 0; i < N; i++) {
    double tmp = a * y[0] - b * y[1];
    y[1] += b * y[0] * dt;
    y[0] += tmp * dt;

    yn.push_back(y);
    
    if(pit != print.end() && *pit == i + 1) {
      std::cout << "t = " << (i + 1) * dt << ", y = [" << y[0] << ", " << y[1] << "]" << std::endl;
      pit++;
    }
  }

  std::vector<double> l = {1.0, 0.0};
  double r = 0.0;
  for(int i = N - 1; i >= 0; i--) {
    // [[a, -b],
    // [b, 0]]
    // [[-a, -b],
    // [b, 0]]
    //
    // [[1.0, 0.0]]

    r -= -l[0] * yn[i][0] * dt;

    double tmp = -a * l[0] - b * l[1];
    l[1] += -b * l[0] * dt;
    l[0] += -tmp * dt;
  }

  std::cout << "l = [" << l[0] << ", " << l[1] << "]" << std::endl;
  std::cout << r << std::endl;
}
