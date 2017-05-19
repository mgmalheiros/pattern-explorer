// computation of inverse chessboard distance field from seed cells
// -> using generic three-reagent approach for automata

define chemical S // state
define chemical N // neighborhood
define chemical C // clock

// -------- prepattern

use chemical S conc 0 diff 0
use chemical N conc 0 diff 0.01
use chemical C conc 0 diff 0

create sqr_grid 15 15

set cell 112 chemical S conc 1

// -------- rules

// periodic clock
rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc +1
rule if C conc == 2 change C conc -2

// clock 0: evaluate neighborhood for state == 1 cells
rule if C conc == 0 and
rule if S conc == 1 change N conc +1

// clock 1: diffusion happens in the neighborhood 

// clock 2: force neighborhood to zero
rule if C conc == 2 change N conc -1

// clock 2: state == n -> state == n+1
rule if C conc == 2 and
rule if S conc != 0 change S conc +1

// clock 2: state == 0 -> state == 1
rule if C conc == 2 and
rule if S conc == 0 and
rule if N conc in 0.01 0.08 change S conc +1 // one to eight neighbor cells

stop at 21
