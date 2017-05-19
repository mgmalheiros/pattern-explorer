// valid center levels for initial concentration and feedback
// are from 4.0 to 6.0, approximately -> lower levels tends to
// fill with maximum values, whereas higher levels fills with zero
// -> as the level is lowered, more area is kept for "up" regions  

define chemical P limit 1

// -------- prepattern

use seed 1

use chemical P conc 0.4 dev 0.1 diff 0.01
//use chemical P conc 0.5 dev 0.1 diff 0.01
//use chemical P conc 0.6 dev 0.1 diff 0.01

create sqr_grid 50 50

// -------- rules

rule from 100 if P conc > 0.4 change P conc +0.01
rule from 100 if P conc < 0.4 change P conc -0.01

//rule from 100 if P conc > 0.5 change P conc +0.01
//rule from 100 if P conc < 0.5 change P conc -0.01

//rule from 100 if P conc > 0.6 change P conc +0.01
//rule from 100 if P conc < 0.6 change P conc -0.01

