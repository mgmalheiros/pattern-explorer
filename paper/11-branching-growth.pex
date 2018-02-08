// branching growth

define chemical V anisotropic
define chemical U

// -------- prepattern

use seed 2
//use seed 4

use chemical U conc 4 dev 1 diff 0.05
use chemical V conc 4 dev 1 diff 0.00625
use polarity 90

create cell 0 0

// -------- rules

rule always react U V scale 0.008 turing alpha 16 beta 12

rule if NEIGHBORS <= 2 and
rule probability 0.008 divide direction   0 dev 0

rule if NEIGHBORS <= 2 and
rule probability 0.0008 divide direction  20 dev 0

rule if NEIGHBORS <= 2 and
rule probability 0.0008 divide direction -20 dev 0

stop at 15000

zoom level 0.447

