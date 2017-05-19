// standard turing pattern + growth + seed cell
// -> concentric pattern is created and maintained

define chemical V
define chemical U limit 5.5

define division_limit 6

// -------- prepattern

use seed 1

use chemical U conc 4 dev 0 diff 0.08
use chemical V conc 4 dev 0 diff 0.01

create hex_circle 45
set cell 950 chemical V conc 0

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

rule from 2000 until 15000 probability 0.0001 divide direction 0 dev 180

//snap from 1000 repeat 15 step 1000

zoom level 1.303
