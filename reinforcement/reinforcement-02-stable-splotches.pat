// simple filter to reinforce higher concentrations and decrease lower ones
// -> makes large and irregular splotches that eventually stabilize 

define chemical P limit 1

// -------- prepattern

use seed 1

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 50 50

// -------- rules

rule if P conc > 0.5 change P conc +0.01
rule if P conc < 0.5 change P conc -0.01
