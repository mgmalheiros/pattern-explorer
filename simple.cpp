/*-------------------------------- INCLUDES --------------------------------*/

#include "simulation.hpp"

/*-------------------------------- MAIN FUNCTION --------------------------------*/

int main()
{

	int u = simulation_define_chemical("U");
	int v = simulation_define_chemical("V");

	// -------- prepattern

    simulation_use_seed(1); // must be called before any other 'use' or 'create' functions

    simulation_use_chemical_concentration(u, 4.0, 2.0);
	simulation_use_chemical_diffusion(u, 0.04F);

	simulation_use_chemical_concentration(v, 4.0, 2.0);
	simulation_use_chemical_diffusion(v, 0.01F);

	simulation_create_square_grid(50, 50);

	// -------- rules

	Rule rule;
	rule.predicate = ALWAYS;
	rule.action = REACT_TU; // turing non-linear reaction
	rule.ac_par[0] = (Parameter) u;
	rule.ac_par[1] = (Parameter) v;
	rule.ac_par[2] = CONSTANT; rule.ac_val[2] = 0.01F; // scale
	rule.ac_par[3] = CONSTANT; rule.ac_val[3] = 16;    // alpha
	rule.ac_par[4] = CONSTANT; rule.ac_val[4] = 12;    // beta
	simulation_add_rule(rule);

	simulation_init();
	simulation_run(1000);
	simulation_done();

	std::cout << '\n';
	std::cout << "iterations: " << simulation.iteration << '\n';
	std::cout << "number of cells: " << simulation.n_cells << '\n';
	std::cout << "number of chemicals: " << simulation.n_chemicals << '\n';
	std::cout << "U min=" << statistics.chem_min[u] << " max=" << statistics.chem_max[u] << '\n';
	std::cout << "V min=" << statistics.chem_min[v] << " max=" << statistics.chem_max[v] << '\n';
	std::cout << '\n';

	Cell& cell = simulation.curr_cells[0];

	std::cout << "cell 0:\n";
	std::cout << "  birth=" << cell.birth << " neighbors=" << cell.neighbors << '\n';
	std::cout << "  position=(" << cell.x << ',' << cell.y << ") fixed=" << (cell.fixed ? "true" : "false") << '\n';
	std::cout << "  polarity=[" << cell.polarity_x << ' ' << cell.polarity_y << "]\n";
	std::cout << "  U concentration=" << cell.conc[u] << " diffusion_rate=" << cell.diff[u] << '\n';
	std::cout << "  V concentration=" << cell.conc[v] << " diffusion_rate=" << cell.diff[v] << '\n';

	return 0;
}
