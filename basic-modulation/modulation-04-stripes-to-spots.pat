// diffusion from row of isolated producer cells
// -> precise gradient (not linear, though)

define chemical V
define chemical U
define chemical A
define chemical P

// -------- prepattern

use chemical U conc 4 dev 0.5 diff 0.0225
use chemical V conc 4 dev 0.5 diff 0.001
use chemical A conc 0 diff 0.05

use chemical P conc 0 diff 0
create sqr_grid 61 60

use chemical P conc 1 diff 0
create sqr_grid 61 1 at 0 -61

// -------- rules

rule if P conc == 1 change A conc  0.05
rule if P conc == 0 change A conc -0.001

rule always map A conc 1 7 to BETA 10 12

rule from 1000 always react U V scale 0.004 turing alpha 16 beta BETA
