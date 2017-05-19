// poison dart frog

define chemical U limit 6.3
define chemical V anisotropic

define time_step 0.5

// -------- prepatterm

use polarity 90

use chemical U conc 4 dev 2 diff 0.30
use chemical V conc 4 dev 2 diff 0.02

create sqr_grid 100 100

set cells 0 to 4999 chemical U conc 4 dev 2 diff 0.011 polarity none
set cells 0 to 4999 chemical V conc 4 dev 2 diff 0.003 polarity none

// -------- rules

rule always react U V scale 0.008 turing alpha 16 beta 12

// -------- coloring

colormap select gradient
colormap slot 20 use color "ffbf00"
colormap slot 25 use color "cyan"
colormap slot 45 use color "202020"

texture size 512 512

stop at 5000

