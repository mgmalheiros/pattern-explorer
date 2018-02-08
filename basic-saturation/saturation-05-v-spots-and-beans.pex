// imposing a saturation limit with other diffusion values  

define chemical U
define chemical V limit 5

// -------- prepattern

use seed 1

// values for V
// limit 7   -> normal spots and beans 
// limit 6   -> mostly spots 
// limit 5   -> mostly round spots 
// limit 4   -> sparse spots and beans 
// limit 3.5 -> large pools 
// limit 3.2 -> numerical problems 

use chemical U conc 4 dev 2 diff 0.06
use chemical V conc 4 dev 2 diff 0.01

create sqr_grid 100 100

// -------- rules

rule always react U V scale 0.01 turing alpha 16 beta 12

