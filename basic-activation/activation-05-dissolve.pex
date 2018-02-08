// chemical P increases above 0.5 concentration, saturated at 1.0
// -> too low change rate make patterns dissolve

define chemical P limit 1

// -------- prepattern

use chemical P conc 0 diff 0.01

create sqr_grid 50 50
set cell 256 chemical P conc 1
set cell 341 chemical P conc 1
set cell 570 chemical P conc 1

// -------- rules

rule if P conc > 0.5 change P conc +0.01
