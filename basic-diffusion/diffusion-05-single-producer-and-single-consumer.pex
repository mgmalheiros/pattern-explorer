// diffusion from single producer and a single consumer
// -> reaches stability after a while, creating a quasi-linear gradient

define chemical A
define chemical P
define chemical C

use chemical A conc 0 diff 0.1
use chemical P conc 0 diff 0
use chemical C conc 0 diff 0

create sqr_grid 30 30

set cell   0 chemical P conc 1
set cell 899 chemical C conc 1

rule if P conc == 1 change A conc  1
rule if C conc == 1 change A conc -1

