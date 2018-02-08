// standard turing pattern + copy + growth + reinforcement
// -> pattern is copied in another chemical, and then maintained
// by a reinforcement mechanism -> overall pattern is kept
//
// - if diffusion of P is too small, noise is added
// - if diffusion of P is too high, pattern is smoothed-out
//
// -> needs extra chemical and tuning of its diffusion/reinforcement rates

define division_limit 6

define chemical P
define chemical V
define chemical U

// -------- prepattern

use seed 1

use chemical P conc 0       diff 0.0004
use chemical V conc 4 dev 2 diff 0.01
use chemical U conc 4 dev 2 diff 0.15

create hex_circle 45

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

// copy pattern from V to P, mapping it to the [0,2] interval
rule from 1999 until 1999 always map V conc 1.0 7.0 to PAT 0.0 2.0
rule from 1999 until 1999 always change P conc PAT

// reinforcement of P
rule from 1999 if P conc in 0.1 0.9 change P conc -0.0004
rule from 1999 if P conc in 1.1 1.9 change P conc +0.0004

rule from 2000 until 15000 probability 0.0001 divide direction 0 dev 180

//snap from 1000 repeat 15 step 1000

zoom level 1.303
