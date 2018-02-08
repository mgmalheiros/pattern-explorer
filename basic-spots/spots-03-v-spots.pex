// turing seeds: applying threshold

define chemical U
define chemical V

// -------- prepattern

// adjusted table for U diffusion rate, V diffusion rate at 0.01 and "s" at 0.01
// overall covergence at 3000
//  
// 0.04 hard round spots (very regular)
// 0.05 soft spots (few short beans)
// 0.06 soft spots and beans
// 0.10 mostly beans
// 0.15 labyrinth

use seed 1

use chemical V conc 4.0 dev 2.0 diff 0.01
use chemical U conc 4.0 dev 2.0 diff 0.04

create sqr_grid 50 50

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

colormap select striped
colormap slot 00 use color "gray"
colormap slot 01 use color "white"

stop at 3000
