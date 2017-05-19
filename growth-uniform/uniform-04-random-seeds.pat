// random seeds (using saturation level) + growth
// -> overall shape is maintaned, but smaller details often are lost

define division_limit 6

define chemical P limit 1

// -------- prepattern

use seed 1

use chemical P conc 0.5 dev 0.5 diff 0.01

create hex_circle 45

// -------- rules

rule if P conc < 0.5 change P conc -0.01
rule if P conc > 0.5 change P conc +0.01

rule from 2000 until 15000 probability 0.0001 divide direction 0 dev 180

//snap from 1000 repeat 15 step 1000

zoom level 1.303
