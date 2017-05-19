// random automata activation -> only adjacent to already activated cells
// -> probability is given by gradient on G

define chemical G limit 1
define chemical P limit 1
define chemical C

// -------- prepattern

use chemical G conc 0 diff 0.001
use chemical P conc 0 diff 0.01
use chemical C conc 0 diff 0

create sqr_grid 50 50
set cell 1274 chemical P conc 1
set cell 1275 chemical P conc 1
set cell 1276 chemical P conc 1

// -------- rules

// clock
rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc -1

// force all to zero
rule if C conc == 1 change P conc -1

// force some to one
rule if C conc == 1 and
rule probability G conc and
rule if P conc in 0.01 0.02 change P conc +2
//rule if P conc in 0.02 0.02 change P conc +2
//rule if P conc in 0.03 0.04 change P conc +2
//rule if P conc in 0.01 0.03 change P conc +2
//rule if P conc in 0.03 0.08 change P conc +2

rule if C conc == 1 and
rule if P conc > 0.9 change P conc +2

rule if P conc == 1 change G conc +0.0001

//colormap select striped
//colormap slot 0 use color "gray"
//colormap slot 1 use color "red"
//colormap slot 2 use color "green"
//colormap slot 3 use color "blue"
//colormap slot 4 use color "yellow"
//colormap slot 99 use color "white"
