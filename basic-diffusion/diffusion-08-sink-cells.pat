// this experiment shows the appearance of sink cells, that sucks all reagents
// -> if negative diffusion is unconstrained, zero-concentration cells will
// keep producing more reagents indefinitely

define chemical A

// medium concentration
use chemical A conc 2 dev 0.0 diff 0.1
create sqr_grid 30 10 at 0 11

// active diffusion mechanism: high concentration
use chemical A conc 3 dev 0.0 diff -0.01
create sqr_grid 30 1 at 0 0

// low concentration
use chemical A conc 1 dev 0 diff 0.1
create sqr_grid 30 10 at 0 -11

stop at 100
