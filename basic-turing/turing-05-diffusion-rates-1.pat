// test different diffusion rates

define chemical U
define chemical V

// -------- prepattern

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.005  dev 0
use chemical V    conc 4 dev 0.5    diff 0.001  dev 0

create sqr_grid 30 30 at -35 35

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.01   dev 0
use chemical V    conc 4 dev 0.5    diff 0.001  dev 0

create sqr_grid 30 30 at 35 35

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.02   dev 0
use chemical V    conc 4 dev 0.5    diff 0.001  dev 0

create sqr_grid 30 30 at -35 -35

use seed 1
use chemical U    conc 4 dev 0.5    diff 0.04   dev 0
use chemical V    conc 4 dev 0.5    diff 0.001  dev 0

create sqr_grid 30 30 at 35 -35

rule always react U V scale 0.004 turing alpha 16 beta 12

