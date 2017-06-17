library(deSolve)
library(tidyverse)
library(ggplot2)

# Run a little ODE
a = 0.0
b = 1.0

func = function(t, state, p) {
  list(c(p[[1]] * state[["x"]] + p[[2]] * state[["y"]], -p[[2]] * state[["x"]]))
}

h = 0.01
times = seq(0, 1, by = h)
stimes = seq(0, 1, by = 10 * h)
y0 = c(x = 1.0, y = 0.0)
fout = as_tibble(ode(y = y0, times = times, func = func, parms = c(a, b))[,])

fout %>% print.data.frame()

fout %>% ggplot(aes(x, y)) +
  geom_point()

# Do finite difference on the parameter
value = function(b) {
  y0 = c(x = 1.0, y = 0.0)
  sum(ode(y = y0, times = stimes, func = func, parms = c(a, b))[2:length(stimes),"x"])
}

print(paste("value:", value(b), ", deriv:", (value(b + .00001) - value(b)) / 0.00001))

