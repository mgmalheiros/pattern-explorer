// diffusion from single isolated producer cell
// concentration modulates turing reaction into concentric circles
// -> try changing starting iteration for reaction

define chemical V
define chemical U
define chemical A
define chemical P

// -------- prepattern

use chemical U conc 4 dev 0.5 diff 0.0225
use chemical V conc 4 dev 0.5 diff 0.001

use chemical A conc 0 diff 0.05
use chemical P conc 0 diff 0

create sqr_grid 41 41
set cell 840 chemical P conc 1

//create sqr_grid 61 61
//set cell 1860 chemical P conc 1

// -------- rules

rule if P conc == 1 change A conc  1.4
rule if P conc == 0 change A conc -0.001

// tested with scale 0.008 and 41x41
rule always map A conc 0 3 to BETA 11 13
//rule always map A conc 0 7 to BETA 9 13
//rule always map A conc 0 3 to BETA 12 16
//rule always map A conc 0 3 to BETA 16 12

// tested with scale 0.008 and 61x61
//rule always map A conc 1 7 to BETA 20 8
//rule always map A conc 3 7 to BETA 12 13

rule from 2000 always react U V scale 0.008 turing alpha 16 beta BETA
