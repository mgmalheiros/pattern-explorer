// chemical P increases above 0.5 concentration
// -> saturation at 1.0 can be made optional on many situations  

define chemical P

// -------- prepattern

use chemical P conc 0 diff 0.01

create sqr_grid 50 50
set cell 256 chemical P conc 1
set cell 341 chemical P conc 1
set cell 570 chemical P conc 1

// -------- rules

rule if P conc in 0.5 1.0 change P conc +0.1
