// modulate scale based on single-cell gradient
// -> regular spots of different sizes

define chemical V
define chemical U
define chemical A
define chemical P

// -------- prepattern

// spots -> depends on large initial variation to spark spot creation 
use chemical U conc 4 dev 1.0 diff 0.025   dev 0
use chemical V conc 4 dev 1.0 diff 0.00625 dev 0

use chemical A conc 0         diff 0.05
use chemical P conc 0         diff 0

create sqr_grid 51 51

set cell 1300 chemical P conc 1

// -------- rules

rule if P conc == 1 change A conc  2.3
rule if P conc == 0 change A conc -0.001

rule always map A conc 0.5 3 to S 0.01 0.04

rule always react U V scale S turing alpha 16 beta 12
