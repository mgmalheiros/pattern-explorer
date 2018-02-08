// cow-like spots

define chemical P limit 1

// -------- prepattern

use seed 2

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 100 100

// -------- rules

rule if P conc > 0.7 change P conc +0.01
rule if P conc < 0.3 change P conc -0.01

colormap select gradient
colormap slot 00 use color "black"
colormap slot 99 use color "white"

stop at 2000

