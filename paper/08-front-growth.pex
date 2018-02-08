// front growth

define chemical V
define chemical U

define time_step 0.1

// -------- prepattern

use polarity -90

use chemical V  conc 4 dev 0.5  diff 0.01
use chemical U  conc 4 dev 0.5  diff 0.08

create sqr_grid 80 1

// -------- rules

rule if AGE in 200 200 divide direction 0 dev 0
rule if AGE <  200 react U V scale 0.01 turing alpha 16 beta 12
rule if AGE in 200 200 change V diff -0.01
rule if AGE in 200 200 change U diff -0.08

colormap select gradient
colormap slot 0 use color "dd9861"
colormap slot 60 use color "3f0400"
colormap slot 80 use color "782e07"

stop at 26000

