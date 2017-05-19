// waiting some iterations for the diffusion to spread-out concentrations:
// if diffusion is slow -> feedback reaction is determinant
// if diffusion is fast -> will smooth-out final pattern

define chemical P limit 1

// -------- prepattern

use seed 1

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 50 50

// -------- rules

//rule from 100 if P conc > 0.5 change P conc +0.01
//rule from 100 if P conc < 0.5 change P conc -0.01

//rule from 200 if P conc > 0.5 change P conc +0.01
//rule from 200 if P conc < 0.5 change P conc -0.01

//rule from 300 if P conc > 0.5 change P conc +0.01
//rule from 300 if P conc < 0.5 change P conc -0.01

rule from 400 if P conc > 0.5 change P conc +0.01
rule from 400 if P conc < 0.5 change P conc -0.01
