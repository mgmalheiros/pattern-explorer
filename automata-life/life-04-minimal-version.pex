//

define chemical L limit 1
define chemical C

// -------- prepattern

use chemical L conc 0 diff 0.01
use chemical C conc 0 diff 0

create sqr_grid 25 25
set cell 526 chemical L conc 1
set cell 527 chemical L conc 1
set cell 528 chemical L conc 1
set cell 553 chemical L conc 1
set cell 577 chemical L conc 1

// -------- rules

rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc -1

// kill all positions by default
rule if C conc == 1 change L conc -1

// a position with three neighbors becomes populated
rule if C conc == 1 and
rule if L conc in 0.025 0.034 change L conc +2

// a position with two or three neighbors survives to next generation
rule if C conc == 1 and
rule if L conc in 0.935 0.954 change L conc +2
