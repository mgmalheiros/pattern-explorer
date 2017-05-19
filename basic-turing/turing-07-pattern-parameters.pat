// turing normalized map
// (by keeping V fixed)

define chemical V
define chemical U

// -------- prepattern

use chemical U  conc 4 dev 0.5  diff 0.005
use chemical V  conc 4 dev 0.5  diff 0.001

// 0.002 tends to uniform concentration
// 0.004 hard round spots
// 0.006 soft spots
// 0.008 soft spots and beans
// 0.01  beans
// 0.02  labyrinth
// 0.04  hard labyrinth 
// 0.06  negative beans
// 0.1   negative spots
// 0.16  negative deep spots

create sqr_grid 50 50

// -------- rules

rule always react U V scale 0.001 turing alpha 16 beta 12

