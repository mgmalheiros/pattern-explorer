// imposing a saturation limit to V create sparse spots

define chemical U
define chemical V limit 3.4

// -------- prepattern

use seed 1

// V saturation limit
// 6   -> like unbounded: normal spots
// 5   -> very round, a bit fewer spots
// 4   -> very few, round spots
// 3.8 -> isolated spots and beans
// 3.4 -> creates pools of different sizes
// 3   -> numeric instability, as U grows unbounded 

use chemical U conc 4 dev 2 diff 0.04
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 50 50

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

