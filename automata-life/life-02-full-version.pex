//

define chemical L limit 1
define chemical C

// -------- prepattern

use chemical L conc 0 diff 0.01
use chemical C conc 0 diff 0

create sqr_grid 30 10
set cell 160 chemical L conc 1
set cell 161 chemical L conc 1
set cell 162 chemical L conc 1
set cell 163 chemical L conc 1
set cell 164 chemical L conc 1
set cell 165 chemical L conc 1
set cell 166 chemical L conc 1
set cell 167 chemical L conc 1
set cell 168 chemical L conc 1
set cell 169 chemical L conc 1

// -------- rules

rule if C conc == 0 change C conc +1
rule if C conc == 1 change C conc -1

// For a space that is "populated":

// Each cell with one or no neighbors dies, as if by solitude.
//rule from 1 if L conc in 0.92 0.93 change L conc -1
rule if C conc == 1 and
rule if L conc in 0.5 0.934 change L conc -1

// Each cell with two or three neighbors survives.
//rule from 1 if L conc in 0.94 0.95 change L conc 1
rule if C conc == 1 and
rule if L conc in 0.935 0.954 change L conc 1

//Each cell with four or more neighbors dies, as if by overpopulation.
//rule from 1 if L conc in 0.96 1.00 change L conc -1
rule if C conc == 1 and
rule if L conc in 0.955 1.00 change L conc -1

// For a space that is "empty" or "unpopulated"

// Each cell with one or two neighbors stays unpopulated.
//rule from 1 if L conc in 0.01 0.02 change L conc -1
rule if C conc == 1 and
rule if L conc in 0.0 0.024 change L conc -1

// Each cell with three neighbors becomes populated.
//rule from 1 if L conc == 0.03 change L conc 1
rule if C conc == 1 and
rule if L conc in 0.025 0.034 change L conc 1

// Each cell with four or more neighbors stays unpopulated.
//rule from 1 if L conc in 0.04 0.08 change L conc -1
rule if C conc == 1 and
rule if L conc in 0.035 0.5 change L conc -1
