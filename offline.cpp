/*-------------------------------- INCLUDES --------------------------------*/

#include <cstdlib>
#include <cstring>

//#include <time.h>

#include "colormap.hpp"
#include "export.hpp"
#include "parser.hpp"
#include "nns_base.hpp"
#include "simulation.hpp"
#include "types.hpp"

/*-------------------------------- MAIN FUNCTION --------------------------------*/

int main(int argc, char *argv[])
{
    if (argc == 1) {
    	std::cout << "usage: offline [OPTION] FILE.pex\n";
    	std::cout << "  --ss         force use of spatial sorting\n";
    	std::cout << "  --kd         force use of k-d tree\n";
    	std::cout << '\n';
    	PAUSE_AND_EXIT;
    }
    argv++; argc--;

    NNSChoice nns_choice = AUTO;
    while ((*argv)[0] == '-') {
    	if (strcmp(*argv, "--ss") == 0) {
    		nns_choice = SPATIAL_SORTING;
    	}
    	else if (strcmp(*argv, "--kd") == 0) {
    		nns_choice = KD_TREE;
    	}
    	else {
    		std::cout << "unknown option '" << *argv << "'\n";
    		PAUSE_AND_EXIT;
    	}
    	argv++; argc--;
    }

    //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_start);

    simulation_use_seed(1); // default to use when not explicitly set

	parser_init(*argv);
	parser_load_pattern();

	colormap_init();
	parser_load_colormap();
	colormap_generate();

	//struct timespec t;
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
	//time_init = (t.tv_sec - time_start.tv_sec) * 1000.0 + (t.tv_nsec - time_start.tv_nsec) * 0.000001;
	//time_sort = time_calc = time_draw = 0;

	simulation_init(nns_choice);

	int it = (simulation.stop_at != -1) ? simulation.stop_at : 10000;
	simulation_run(it);
	std::cout << '\n';
    std::cout << "ended after " << simulation.iteration << " iterations\n";
	export_vector(0);
	std::cout << "vector output saved with " << simulation.n_cells << " cells\n";

	simulation_done();

    return 0;
}
