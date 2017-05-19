// simple diffusion between groups of impermeable cells

define chemical A

use chemical A conc 0 diff 0
create sqr_grid 30 10 at 0 -20

use chemical A conc 0 diff 0.01
create sqr_grid 30 10 at 0 0

use chemical A conc 0 diff 0.00
create sqr_grid 30 10 at 0 20

set cell 455 chemical A conc 100 diff 0.01
