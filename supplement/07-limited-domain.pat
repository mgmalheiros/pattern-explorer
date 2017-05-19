// expading domain

define chemical U
define chemical V

define domain 180 180

// -------- prepattern

use chemical V conc 4 dev 0.5 diff 0.001
use chemical U conc 4 dev 0.5 diff 0.008
use polarity -90

create sqr_grid 20 1 dev 0.1

// -------- rules

rule always react U V scale 0.001 turing alpha 16 beta 12
rule if AGE in 500 500 divide direction 0 dev 0

stop at 60000

zoom level 1.0
