library(deSolve)
library(tidyverse)
library(ggplot2)

a = -1.0

func = function(t, state, p) {
  list(c(p[[1]] * state[["y"]]^2))
}

h = 0.05
times = seq(0, 5, by = h)
y0 = c(y = 1.0)
fout = as_tibble(ode(y = y0, times = times, func = func, parms = c(a))[,])

fout %>% ggplot(aes(time, y)) +
  geom_point()

fout %>% print.data.frame()

value = function(a) {
  y0 = c(y = 1.0)
  sum(ode(y = y0, times = c(head(times, 1), 2.0, tail(times, 1)), func = func, parms = c(a, b))[2:3,"y"])
}

(value(a + .00001) - value(a)) / 0.00001
