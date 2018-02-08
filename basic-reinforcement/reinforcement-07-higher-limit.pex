// raising the concentration limit makes borders more thick, with several 
// intermediate levels; the feedback rates are like in other examples

define chemical P limit 2

// -------- prepattern

use seed 1

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 50 50

// -------- rules

//rule from 100 if P conc > 0.5 change P conc +0.01
//rule from 100 if P conc < 0.5 change P conc -0.02

rule from 100 if P conc > 0.5 change P conc +0.01
rule from 100 if P conc < 0.5 change P conc -0.04

//rule from 100 if P conc > 0.5 change P conc +0.01
//rule from 100 if P conc < 0.5 change P conc -0.08
