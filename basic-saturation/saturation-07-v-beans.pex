// imposing a saturation limit with other diffusion values  

// values for V
// limit 10  -> normal long beans
// limit 8   -> shorter beans
// limit 6   -> beans and spots
// limit 5   -> more spots, fewer beans
// limit 4   -> distribution more sparse
// limit 3.5 -> irregular pools
// limit 3   -> numerical problems

define chemical U
define chemical V limit 4

// -------- prepattern

use seed 1

use chemical U conc 4 dev 2 diff 0.10
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 100 100

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12
 
