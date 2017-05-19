// chemical P increases within limited interval, saturated at 1.0
// -> lower change interval makes centers and boundaries to
// retain higher concentrations   

define chemical P limit 1

// -------- prepattern

use chemical P conc 0 diff 0.01

create sqr_grid 50 50
set cell 256 chemical P conc 1
set cell 341 chemical P conc 1
set cell 570 chemical P conc 1

// -------- rules

rule if P conc in 0.01 0.1 change P conc +0.2
