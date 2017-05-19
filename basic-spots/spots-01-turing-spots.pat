// turing seeds: U has more precise spikes when using "hard stops"
// -> higher initial deviations for U and V concentration increase convergence speed
// -> both higher diffusion rates and "s" parameter increase convergence speed

define chemical U
define chemical V

// -------- prepattern

use seed 1

// adjusted table for U diffusion rate, V diffusion rate at 0.01 and "s" at 0.01
// overall covergence at 3000
//  
// 0.04 hard round spots (very regular)
// 0.05 soft spots (few short beans)
// 0.06 soft spots and beans
// 0.10 mostly beans
// 0.15 labyrinth

use chemical U conc 4.0 dev 2.0 diff 0.04
use chemical V conc 4.0 dev 2.0 diff 0.01

create sqr_grid 50 50

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12
