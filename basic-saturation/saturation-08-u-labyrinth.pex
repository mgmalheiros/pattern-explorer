// imposing a saturation limit with other diffusion values  

define chemical U limit 4.6 
define chemical V 

// values for U
// limit 6   -> normal labyrinth
// limit 5   -> labyrinth with larger tips
// limit 4.8 -> stripes get fatter
// limit 4.6 -> sparse negative beans and round spots
// limit 4.4 -> sparse negative spots only
// limit 4.1 -> pattern disapeer

// -------- prepattern

use seed 1

use chemical U conc 4 dev 2 diff 0.15
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 100 100

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

