// this particular combination generates few isolated islands

define chemical P limit 1

// -------- prepattern

use seed 1

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 50 50

// -------- rules

rule if P conc > 0.8 change P conc +0.03
rule if P conc < 0.6 change P conc -0.01
