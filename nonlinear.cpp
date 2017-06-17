//#include <stan/math.hpp>
#include <iostream>
#include <vector>
#include <cmath>

//using namespace stan::math;

int main(int argc, char **argv) {
  double a = -1.0;

  double dt = 0.001;
  int N = int(5.0 / dt);
  std::vector<int> print = {10, 20, 40, 100, N};
  std::vector<double> yn(N);

  double y = 1.0;

  std::vector<int>::iterator pit = print.begin();
  for(int i = 0; i < N; i++) {
    y += a * y * y * dt;

    yn[i] = y;
    
    if(pit != print.end() && *pit == i + 1) {
      std::cout << "t = " << (i + 1) * dt << ", y = " << y << std::endl;
      pit++;
    }
  }

  std::cout << "total : " << yn[(int)(2.0 / dt) - 1] + y << std::endl;

  double l = 1.0;
  double r = 0.0;
  for(int i = N - 1; i >= 0; i--) {
    r -= -yn[i] * yn[i] * l * dt;

    if((int)(2.0 / dt) - 1 == i)
      l += 1.0;
    
    l -= -a * 2 * yn[i] * l * dt;
  }

  std::cout << "l = " << l << std::endl;
  std::cout << "grad = " << r << std::endl;
}
