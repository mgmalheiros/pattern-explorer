// by using chemical saturation, it is possible to create a single-chemical gradient
// -> care must be taken to prevent non-producer cells from reaching the limit too

define chemical G limit 1

// -------- prepattern

use chemical G conc 0 diff 0.01

create sqr_grid 50 50
set cell 256 chemical G conc 1
set cell 342 chemical G conc 1
set cell 570 chemical G conc 1

// -------- rules

rule if G conc == 1 change G conc +1
rule if G conc != 1 change G conc -0.0001

