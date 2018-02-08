// a negative diffusion will produce more reagent in some circustances
// the chemical will be pushed onto higher concentration areas, even
// after the concentration at membrane drops to zero

define chemical A

// high concentration
use chemical A conc 3 dev 0.0 diff 0.1
create sqr_grid 30 10 at 0 11

// active diffusion mechanism: low concentration
use chemical A conc 1 dev 0.0 diff -0.01
create sqr_grid 30 1 at 0 0

// medium concentration
use chemical A conc 2 dev 0 diff 0.1
create sqr_grid 30 10 at 0 -11

stop at 100
