// using a single-cell gradient to establish polarization
// -> using gradient as direction for anisotropic diffusion
// -> spotted pattern with BOTH U and V are anisotropic

define chemical V anisotropic
define chemical U anisotropic 
define chemical A
define chemical P

// -------- prepattern

// spots -> depends on large initial variation to spark spot creation 
// try varying U diffusion rate from 0.022 to 0.030 -> less regular spots, but faster to emerge

use chemical U conc 4 dev 1 diff 0.024
use chemical V conc 4 dev 1 diff 0.00625
use chemical A conc 0       diff 0.05
use chemical P conc 0       diff 0

create sqr_grid 51 51
set cell 1300 chemical P conc 1

// -------- rules

rule if P conc == 1 change A conc  2.3
rule if P conc == 0 change A conc -0.001

rule always polarize A conc

rule always react U V scale 0.008 turing alpha 16 beta 12

