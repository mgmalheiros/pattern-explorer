// simple diffusion from single producer cell
// -> concentration increases without bounds

define chemical A
define chemical P

use chemical A conc 0 diff 0.05
use chemical P conc 0 diff 0

create sqr_grid 30 30

set cell 0 chemical P conc 1

rule if P conc == 1 change A conc 0.8
