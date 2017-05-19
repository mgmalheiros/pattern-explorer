// using a single-cell gradient to establish polarization
// -> using gradient as direction for anisotropic diffusion
// -> radial stripped pattern when JUST V is anisotropic
// -> concentric stripped pattern when JUST U is anisotropic

define chemical V //anisotropic
define chemical U anisotropic
define chemical A
define chemical P

// spots setting, but turns into stripes!!!
use chemical U conc 4 dev 1.0 diff 0.06    // try varying U diffusion rate
use chemical V conc 4 dev 1.0 diff 0.00625
use chemical A conc 0         diff 0.05
use chemical P conc 0         diff 0

create sqr_grid 51 51

set cell 1300 chemical P conc 1

rule if P conc >= 1 change A conc  2.3
rule if P conc <  1 change A conc -0.001

rule always polarize A conc

rule always react U V scale 0.008 turing alpha 16 beta 12
