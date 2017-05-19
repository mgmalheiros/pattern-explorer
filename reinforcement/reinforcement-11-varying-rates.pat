// using the same diffusion and feedback rates results in almost the same pattern
// -> the difference is on the convergence speed 

define chemical P limit 1
define chemical N

// -------- prepattern

use seed 1
use chemical P conc 0.5 dev 0.5 diff 0.1
use chemical N conc 0
create sqr_grid 50 50 at -55 +55

use seed 1
use chemical P conc 0.5 dev 0.5 diff 0.05
use chemical N conc 1
create sqr_grid 50 50 at +55 +55

use seed 1
use chemical P conc 0.5 dev 0.5 diff 0.01
use chemical N conc 2
create sqr_grid 50 50 at -55 -55

use seed 1
use chemical P conc 0.5 dev 0.5 diff 0.005
use chemical N conc 3
create sqr_grid 50 50 at +55 -55

// -------- rules

rule if N conc == 0 and
rule if P conc > 0.5 change P conc +0.1
rule if N conc == 0 and
rule if P conc < 0.5 change P conc -0.1

rule if N conc == 1 and
rule if P conc > 0.5 change P conc +0.05
rule if N conc == 1 and
rule if P conc < 0.5 change P conc -0.05

rule if N conc == 2 and
rule if P conc > 0.5 change P conc +0.01
rule if N conc == 2 and
rule if P conc < 0.5 change P conc -0.01

rule if N conc == 3 and
rule if P conc > 0.5 change P conc +0.005
rule if N conc == 3 and
rule if P conc < 0.5 change P conc -0.005
