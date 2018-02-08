// define polarity from single cell gradient

define chemical A
define chemical P

// -------- prepattern

use chemical A conc 0 diff 0.05
use chemical P conc 0 diff 0

create sqr_grid 51 51
set cell 1300 chemical P conc 1

// -------- rules

rule if P conc == 1 change A conc  1.5
rule if P conc == 0 change A conc -0.001

rule always polarize A conc

