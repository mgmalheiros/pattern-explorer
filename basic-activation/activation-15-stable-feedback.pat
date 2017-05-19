// after some time of activation, a feedback mechanism is used to stabilize
// the current wave

define chemical P limit 1

// -------- prepattern

use chemical P conc 0 diff 0.01

create sqr_grid 50 50
set cell 256 chemical P conc 1
set cell 341 chemical P conc 1
set cell 570 chemical P conc 1

// -------- rules

//rule until 100 if P conc > 0.1  change P conc +0.1
rule until 100 if P conc > 0.01 change P conc +0.01

rule from 100 if P conc > 0.5 change P conc +0.01
rule from 100 if P conc < 0.5 change P conc -0.01
