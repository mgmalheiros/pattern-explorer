// simple diffusion from single source
// -> stability is reached when each cell has 1.111 concentration, that is,
//    1.111 * 900 cells = 1000 (initial concentration on cell 0)

define chemical A

use chemical A conc 0 diff 0.1

create sqr_grid 30 30

set cell 0 chemical A conc 1000 diff 0.1
