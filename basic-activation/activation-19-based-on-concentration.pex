// using explicit producer (P) cells that create a chemical gradient (G)
// -> random activation based on concentration happens only within some isolevel, and not at tips
// -> producer cells are not connected and final concentration is mostly round
// -> choice of activation interval has little effect on result

define chemical G limit 1
define chemical P limit 1

// -------- prepattern

use chemical G conc 0 diff 0.01
use chemical P conc 0 diff 0

create sqr_grid 50 50
set cell 1275 chemical P conc 1
set cell 1276 chemical P conc 1

// -------- rules

// gradient
rule if P conc == 1 change G conc +1
rule if P conc == 0 change G conc -0.0001

// create more producers
rule probability 0.001 and
rule if G conc in 0.50 0.55 change P conc +1
