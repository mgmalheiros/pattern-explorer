// wireworld

define chemical S
define chemical N
define chemical C

// -------- prepattern

use chemical S conc 0 diff 0
use chemical N conc 0 diff 0.01
use chemical C conc 0 diff 0

create sqr_grid 15 15

set cells 77 to 79 chemical S conc 3
set cell 61 chemical S conc 3
set cell 65 chemical S conc 3
set cell 46 chemical S conc 3
set cells 50 to 58 chemical S conc 3
set cell 31 chemical S conc 3
set cell     35 chemical S conc 3
set cell 17 chemical S conc 2
set cell 18 chemical S conc 1
set cell 19 chemical S conc 3

// -------- rules

// periodic clock
rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc +1
rule if C conc == 2 change C conc -2

// clock 0: mark head [1] in the neighborhood
rule if C conc == 0 and
rule if S conc == 1 change N conc +1

// clock 1: diffusion happens in the neighborhood 

// clock 2: force neighborhood to zero 
rule if C conc == 2 change N conc -1

// clock 2: empty [0] -> empty [0]

// clock 2: head [1] -> tail [2]
rule if C conc == 2 and
rule if S conc == 1 change S conc +1

// clock 2: tail [2] -> wire [3]
rule if C conc == 2 and
rule if S conc == 2 change S conc +1

// clock 2: wire [3] -> head [1] if exactly one or two neighbor cells are heads, otherwise keep as wire
rule if C conc == 2 and
rule if S conc == 3 and
rule if N conc in 0.01 0.02 change S conc -2

colormap select striped
colormap slot 00 use color "303030" // empty
colormap slot 25 use color "0080ff" // head
colormap slot 50 use color "ffffff" // tail
colormap slot 75 use color "ff8000" // wire

stop at 72
