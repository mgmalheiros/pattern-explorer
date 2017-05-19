// imposing a saturation limit with other diffusion values  

define chemical U
define chemical V limit 4

// -------- prepattern

use seed 1

// values for V
// limit 14   -> normal labyrinth
// limit 12   -> stripes get fatter
// limit 10   -> labyrinth starts to break apart 
// limit 8    -> mostly long beans
// limit 6    -> beans and round, larger spots
// limit 4    -> irregular beans and spots
// limit 3.5  -> irregular pools
// limit 3    -> numerical problems

use chemical U conc 4 dev 2 diff 0.15
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 100 100

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12
 
