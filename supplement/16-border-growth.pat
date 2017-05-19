// border growth

define chemical V anisotropic
define chemical U

// -------- prepattern

use chemical U conc 4 dev 1 diff 0.05
use chemical V conc 4 dev 1 diff 0.00625 

create cell 0 0

// -------- rules

rule always react U V scale 0.004 turing alpha 16 beta 12

rule if NEIGHBORS <= 3 and

rule probability 0.004 divide direction 0 dev 180 // left image

//rule probability 0.004 divide direction 10 dev 0 // right image

stop at 16000

zoom level 1.127

