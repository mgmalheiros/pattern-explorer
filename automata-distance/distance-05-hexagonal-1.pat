// computation of hexagonal distance field from seed cells
// -> based on the generic approach for automata

define chemical S // state
define chemical D // distance
define chemical N // neighborhood
define chemical C // clock

// -------- prepattern

use chemical S conc 0 diff 0
use chemical D conc 1 diff 0
use chemical N conc 0 diff 0.01
use chemical C conc 0 diff 0

create hex_grid 17 17

set cell 144 chemical S conc 1

// -------- rules

// periodic clock
rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc +1
rule if C conc == 2 change C conc -2

// clock 0: evaluate neighborhood for state != 0 cells
rule if C conc == 0 and
rule if S conc != 0 change N conc +1

// clock 0: increase distance counter
rule if C conc == 0 change D conc +1

// clock 1: diffusion happens in the neighborhood 

// clock 2: force neighborhood to zero
rule if C conc == 2 change N conc -1

// clock 2: state == n -> state == n

// clock 2: state == 0 -> state == current distance
rule if C conc == 2 and
rule if S conc == 0 and
rule if N conc in 0.01 0.08 change S conc D conc // three to eight neighbor cells

stop at 24
