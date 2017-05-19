// diffusion from three isolated producer cells
// -> approximate linear gradient

define chemical V
define chemical U
define chemical A
define chemical P

// -------- prepattern

use chemical U conc 4 dev 0.5 diff 0.0225
use chemical V conc 4 dev 0.5 diff 0.001

use chemical A conc 0 diff 0.05
use chemical P conc 0 diff 0

create sqr_grid 61 61

set cell 0 chemical P conc 1
set cell 30 chemical P conc 1
set cell 60 chemical P conc 1

// -------- rules

rule if P conc == 1 change A conc  1.0
rule if P conc == 0 change A conc -0.001

rule always map A conc 1 4 to BETA 16 8
//rule always map A conc 0 7 to BETA 9 13
//rule always map A conc 0 3 to BETA 12 16
//rule always map A conc 0 3 to BETA 16 12
//rule always map A conc 3 7 to BETA 11 13
//rule always map A conc 3 5 to BETA 11 10

rule from 2000 always react U V scale 0.002 turing alpha 16 beta BETA

