// imposing a saturation limit with other diffusion values  

define chemical U limit 5
define chemical V

// -------- prepattern

use seed 1

// values for U
// limit 6.5 -> normal long beans
// limit 5.6 -> brainy labyrinth, not interconnected
// limit 5.4 -> denser labyrinth, more connected
// limit 5.2 -> stripes get fatter, start to merge
// limit 5.0 -> few segmented areas, rivers are defined
// limit 4.8 -> pattern disapeer

use chemical U conc 4 dev 2 diff 0.10
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 100 100

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

