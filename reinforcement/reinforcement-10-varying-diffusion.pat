// if diffusion is higher than feedback, then stable pattern are larger and rounder
// if diffusion is lower, then patterns will be more irregular  

define chemical P limit 1

// -------- prepattern

use seed 1
use chemical P conc 0.5 dev 0.1 diff 0.1
create sqr_grid 50 50 at -55 +55

use seed 1
use chemical P conc 0.5 dev 0.1 diff 0.05
create sqr_grid 50 50 at +55 +55

use seed 1
use chemical P conc 0.5 dev 0.1 diff 0.01
create sqr_grid 50 50 at -55 -55

use seed 1
use chemical P conc 0.5 dev 0.1 diff 0.005
create sqr_grid 50 50 at +55 -55

// -------- rules

rule if P conc > 0.5 change P conc +0.01
rule if P conc < 0.5 change P conc -0.01
