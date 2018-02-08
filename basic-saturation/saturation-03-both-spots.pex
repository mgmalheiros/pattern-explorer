// imposing a saturation limit to both U and V  

define chemical U limit 6.3
define chemical V limit 6.3

// -------- prepattern

use seed 1

// U and V saturation limits
// 8   -> like unbounded: normal spots
// 7   -> round + elliptic spots
// 6.5 -> round small and large spots
// 6.3 -> small spots disappear after a while, creating voronoi-like polygons
// 6   -> all spots eventually merge themselves 

use chemical U conc 4 dev 2 diff 0.04
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 50 50

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

