// modulate scale based on single-cell gradient

define chemical V
define chemical U
define chemical A
define chemical P

// -------- prepattern

use chemical U conc 4 dev 0.1 diff 0.0225
use chemical V conc 4 dev 0.1 diff 0.001

use chemical A conc 0         diff 0.05
use chemical P conc 0         diff 0

create sqr_grid 51 51

set cell 1300 chemical P conc 1

// -------- rules

rule if P conc == 1 change A conc  2.0
rule if P conc == 0 change A conc -0.001

rule always map A conc 0.1 6 to S 0.001 0.008

rule always react U V scale S turing alpha 16 beta 12
