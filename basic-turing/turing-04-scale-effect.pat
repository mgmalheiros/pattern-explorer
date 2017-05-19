// test proportional change of diffusion rates 
// -> has the exact same effect as the reaction scale factor

define chemical U
define chemical V

// -------- prepattern

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.01    dev 0
use chemical V    conc 4 dev 0.5    diff 0.0005  dev 0

create sqr_grid 30 30 at -35 35

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.02   dev 0
use chemical V    conc 4 dev 0.5    diff 0.001  dev 0

create sqr_grid 30 30 at 35 35

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.04   dev 0
use chemical V    conc 4 dev 0.5    diff 0.002  dev 0

create sqr_grid 30 30 at -35 -35

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.08   dev 0
use chemical V    conc 4 dev 0.5    diff 0.004  dev 0

create sqr_grid 30 30 at 35 -35

rule always react U V scale 0.004 turing alpha 16 beta 12

