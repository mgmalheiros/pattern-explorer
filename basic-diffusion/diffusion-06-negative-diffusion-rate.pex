// a negative diffusion rate makes possible to simulate pumping or forced diffusion:
// transport chemicals from a lower concentration region to a higher concentration one
// -> works until membrane has intermediate concentration levels
// -> after that, membrane cells continue pushing continually their own reagents, not
// stopping even after reaching zero concentration (in that case, producing more)  

define chemical A

// high concentration
use chemical A conc 3 dev 0.0 diff 0.1
create sqr_grid 30 10 at 0 11

// active diffusion mechanism: medium concentration
use chemical A conc 2 dev 0.0 diff -0.01
create sqr_grid 30 1 at 0 0

// low concentration
use chemical A conc 1 dev 0 diff 0.1
create sqr_grid 30 10 at 0 -11

stop at 100
