// automata-like activation, based on precise diffusion values

use chemical P limit 1
use chemical C 

// -------- prepattern

use chemical P conc 0 diff 0.01
use chemical C conc 0 diff 0

create sqr_grid 50 50
set cell 256 chemical P conc 1
set cell 342 chemical P conc 1
set cell 570 chemical P conc 1

// -------- rules

// clock

rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc -1

// force all to zero

rule if C conc == 1 change P conc -1

// force some to one

rule if C conc == 1 and
rule if P conc == 0.01 change P conc +2

rule if C conc == 1 and
rule if P conc == 0.03 change P conc +2

rule if C conc == 1 and
rule if P conc > 0.5 change P conc +2
