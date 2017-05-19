// varyaing deviation has little effect on final pattern
// -> higher devations usually give faster convergence

define chemical P limit 1

// -------- prepattern

use seed 1
use chemical P conc 0.5 dev 0.01 diff 0.01
create sqr_grid 50 50 at -55 +55

use seed 1
use chemical P conc 0.5 dev 0.1 diff 0.01
create sqr_grid 50 50 at +55 +55

use seed 1
use chemical P conc 0.5 dev 0.25 diff 0.01
create sqr_grid 50 50 at -55 -55

use seed 1
use chemical P conc 0.5 dev 0.5 diff 0.01
create sqr_grid 50 50 at +55 -55

// -------- rules

rule if P conc > 0.5 change P conc +0.01
rule if P conc < 0.5 change P conc -0.01
