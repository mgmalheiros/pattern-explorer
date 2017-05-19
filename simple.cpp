/*-------------------------------- INCLUDES --------------------------------*/

#include "simulation.hpp"

/*-------------------------------- MAIN FUNCTION --------------------------------*/

int main()
{
	int u = simulation_define_chemical("U");
	int v = simulation_define_chemical("V");

	// -------- prepattern

	simulation_use_chemical_concentration(u, 4.0, 2.0);
	simulation_use_chemical_diffusion(u, 0.04);

	simulation_use_chemical_concentration(v, 4.0, 2.0);
	simulation_use_chemical_diffusion(v, 0.01);

	simulation_create_square_grid(51, 51);

	// -------- rules

	Rule rule;
	rule.predicate = ALWAYS;
	rule.action = REACT_TU; // turing reaction
	rule.ac_par[0] = (Parameter) u;
	rule.ac_par[1] = (Parameter) v;
	rule.ac_par[2] = CONSTANT; rule.ac_val[2] = 0.01; // scale
	rule.ac_par[3] = CONSTANT; rule.ac_val[3] = 16;   // alpha
	rule.ac_par[4] = CONSTANT; rule.ac_val[4] = 12;   // beta
	simulation_add_rule(rule);

	simulation_init();
	simulation_run(1000);
	simulation_done();

	std::cout << "iterations: " << simulation.iteration << '\n';
	std::cout << "number of cells: " << simulation.n_cells << '\n';
	std::cout << "number of chemicals: " << simulation.n_chemicals << '\n';
	std::cout << "U min=" << statistics.chem_min[u] << " max=" << statistics.chem_max[u] << '\n';
	std::cout << "V min=" << statistics.chem_min[v] << " max=" << statistics.chem_max[v] << '\n';

	return 0;
}
