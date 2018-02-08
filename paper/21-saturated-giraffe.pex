// giraffe

define chemical U limit 6.3
define chemical V limit 6.3

// -------- prepattern

use seed 1

use chemical U conc 4 dev 3 diff 0.04
use chemical V conc 4 dev 3 diff 0.01

create sqr_grid 100 100 wrap

// -------- rules

rule always react U V scale 0.005 turing alpha 16 beta 12

colormap select gradient
colormap slot 60 use color "white"
colormap slot 75 use color "5b3504"

stop at 38000

