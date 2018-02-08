// simple filter to keep only the cells with higher concentration levels
// -> makes randomly distributed seeds

define chemical P limit 1

// -------- prepattern

use seed 1

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 50 50

// -------- rules

rule if P conc > 0.95 change P conc +0.1
rule if P conc < 0.95 change P conc -0.1

//rule if P conc > 0.9 change P conc +0.1
//rule if P conc < 0.9 change P conc -0.1

//rule if P conc > 0.5 change P conc +0.1
//rule if P conc < 0.5 change P conc -0.1
