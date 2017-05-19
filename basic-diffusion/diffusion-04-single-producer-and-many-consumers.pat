// diffusion from single producer cell; all other cells are consumers
// -> reaches stability after a while, creating a radial gradient

define chemical A
define chemical P

use chemical A conc 0 diff 0.05
use chemical P conc 0 diff 0

create sqr_grid 30 30

set cell 465 chemical P conc 1

rule if P conc == 1 change A conc  0.8
rule if P conc == 0 change A conc -0.001
