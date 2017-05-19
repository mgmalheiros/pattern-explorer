// imposing a saturation limit with other diffusion values  

define chemical U limit 5.6
define chemical V

// -------- prepattern

use seed 1

// values for U
// limit 7   -> normal spots and beans 
// limit 6.5 -> spots and longer beans 
// limit 6.2 -> larger spots and beans 
// limit 5.8 -> medium blobs, begin to merge 
// limit 5.6 -> larger blobs, non-convex 
// limit 5.4 -> pattern disapeer 

use chemical U conc 4 dev 2 diff 0.06
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 100 100

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

