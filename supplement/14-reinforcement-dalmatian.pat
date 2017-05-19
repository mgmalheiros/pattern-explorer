// dalmatian-liks spots

define chemical P limit 1

// -------- prepattern

use seed 4

use chemical P conc 0.5 dev 0.5 diff 0.01

create sqr_grid 100 100

// -------- rules

rule if P conc > 0.8 change P conc +0.03
rule if P conc < 0.6 change P conc -0.01

colormap select gradient
colormap slot 00 use color "white"
colormap slot 99 use color "black"

stop at 2000

