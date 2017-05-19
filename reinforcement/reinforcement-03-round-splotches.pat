// when the interval (lower and higher cuttof levels) increases
// we get larger and rounder stable splotches 

define chemical P limit 1

// -------- prepattern

use seed 0

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 50 50

// -------- rules

//rule if P conc > 0.6 change P conc +0.01
//rule if P conc < 0.4 change P conc -0.01

rule if P conc > 0.7 change P conc +0.01
rule if P conc < 0.3 change P conc -0.01

//rule if P conc > 0.8 change P conc +0.01
//rule if P conc < 0.2 change P conc -0.01
