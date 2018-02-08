// laplacian growth

define chemical P limit 1
define chemical G
define chemical C

// -------- prepattern

use chemical P conc 0   diff 0.01
use chemical G conc 0.1 diff 0.1
use chemical C conc 0   diff 0

create sqr_grid 100 100
set cell 4949 chemical P conc 1
set cell 4950 chemical P conc 1
set cell 4951 chemical P conc 1

// -------- rules

// clock
rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc -1

// force all to zero
rule if C conc == 1 change P conc -1

// some non-active neighbors will be activated
rule if C conc == 1 and
rule probability G conc and
rule if P conc in 0.03 0.04 change P conc +2

// keep previously activated
rule if C conc == 1 and
rule if P conc > 0.9 change P conc +2

// change probability gradient
rule if P conc == 0 change G conc +0.0001
rule if P conc == 1 change G conc -0.01

stop at 3200
