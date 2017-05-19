// center growth

define chemical V
define chemical U

// -------- prepattern

use chemical V conc 4 dev 0.5 diff 0.001
use chemical U conc 4 dev 0.5 diff 0.008

create cell 0 0

// -------- rules

rule always react U V scale 0.001 turing alpha 16 beta 12
rule if AGE in 1 1 divide direction 137.5 dev 0

stop at 12000

zoom level 1.309

