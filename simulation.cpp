/*-------------------------------- INCLUDES --------------------------------*/

#include <cstdlib>
#include <cstdio>
#include <iomanip>

//#include <time.h>

#include "nns_base.hpp"
#include "types.hpp"

#include "simulation.hpp"

/*-------------------------------- EXPORTED VARIABLES --------------------------------*/

NNS *nns;
Simulation simulation;
Statistics statistics;

//float time_nns_setup, /*time_evaluate, time_nns_query, time_interact,*/ time_calculate, time_total;

/*-------------------------------- LOCAL TYPES --------------------------------*/

struct CellParameters {
	float polarity;
	float polarity_dev;
	float chem_conc[MAX_CHEMICALS];
	float chem_conc_dev[MAX_CHEMICALS];
	float chem_diff[MAX_CHEMICALS];
	float chem_diff_dev[MAX_CHEMICALS];

	CellParameters() {
		reset();
	}

	void reset() {
		polarity = FLT_MAX; // none
		polarity_dev = 0;
		for (int i = 0; i < MAX_CHEMICALS; i++) {
			chem_conc[i] = 0;
			chem_conc_dev[i] = 0;
			chem_diff[i] = 0;
			chem_diff_dev[i] = 0;
		}
	}
};

/*-------------------------------- LOCAL VARIABLES --------------------------------*/

static CellParameters cell_parameters;

static int  nns_sqr_dim_x = 0;
static int  nns_sqr_dim_y = 0;
static bool nns_sqr_wrap = false;

static int rand_t[344];
static int rand_i;

/*-------------------------------- RANDOM NUMBER FUNCTIONS --------------------------------*/

// portable and glibc-compatible random number generator
// adapted from https://www.mscs.dal.ca/~selinger/random/

static void rand_seed(unsigned int seed)
{
    rand_t[0] = seed;
    for (int i = 1; i < 31; i++)
    {
        rand_t[i] = (16807LL * rand_t[i - 1]) % 2147483647;
        if (rand_t[i] < 0) {
            rand_t[i] += 2147483647;
        }
    }
    for (int i = 31; i < 34; i++)
    {
        rand_t[i] = rand_t[i - 31];
    }
    for (int i = 34; i < 344; i++)
    {
        rand_t[i] = rand_t[i - 31] + rand_t[i - 3];
    }
    rand_i = 0;
}

static int rand_int()
{
    int r = rand_t[rand_i % 344] = rand_t[(rand_i + 313) % 344] + rand_t[(rand_i + 341) % 344];
    rand_i = (rand_i + 1) % 344;

    return ((unsigned int) r) >> 1;
}

static float rand_range(float min, float max)
{
    return min + (max - min) * ((float) rand_int() / 2147483647);
}

static float rand_dev(float value, float deviation)
{
	if (deviation == 0) {
		return value;
	}
	return rand_range(value - deviation, value + deviation);
}

/*-------------------------------- DEFINE FUNCTIONS --------------------------------*/

int simulation_define_chemical(std::string name, float limit, bool anisotropic)
{
	int ch = simulation.new_chemical();

	simulation.chemicals[ch].name = name;
	simulation.chemicals[ch].limit = limit;
	simulation.chemicals[ch].anisotropic = anisotropic;

	return ch;
}

void simulation_define_division_limit(int division_limit)
{
	simulation.division_limit = division_limit;
}

void simulation_define_domain(float width, float height)
{
	simulation.domain_xmin = - width  / 2;
	simulation.domain_xmax =   width  / 2;
	simulation.domain_ymin = - height / 2;
	simulation.domain_ymax =   height / 2;
}

void simulation_define_time_step(float time_step)
{
	simulation.time_step = time_step;
}

void simulation_define_mirror_pair(CellId id1, CellId id2)
{
	simulation.mirroring = true;
	simulation.mirror_list.push_back(std::make_pair(id1, id2));
}

/*-------------------------------- USE FUNCTIONS --------------------------------*/

void simulation_use_chemical_concentration(int chemical, float value, float deviation)
{
	cell_parameters.chem_conc[chemical] = value;
	cell_parameters.chem_conc_dev[chemical] = deviation;
}

void simulation_use_chemical_diffusion(int chemical, float value, float deviation)
{
	cell_parameters.chem_diff[chemical] = value;
	cell_parameters.chem_diff_dev[chemical] = deviation;
}

void simulation_use_polarity(float angle, float deviation)
{
	cell_parameters.polarity = angle;
	cell_parameters.polarity_dev = deviation;
}

void simulation_use_seed(int seed)
{
	if (seed == 0) {
		rand_seed((unsigned int) time(NULL));
	}
	else {
		rand_seed(seed);
	}
}

/*-------------------------------- CREATE FUNCTIONS --------------------------------*/

// NOTE: this function is called only when setting up the simulation, that is, when defining the starting pattern

CellId simulation_create_cell(float x, float y, bool fixed)
{
	CellId id = simulation.new_cell();

	simulation.curr_cells[id].birth = 0;
	simulation.curr_cells[id].neighbors = 0;
	simulation.curr_cells[id].x = x;
	simulation.curr_cells[id].y = y;
	if (cell_parameters.polarity != FLT_MAX) {
		float angle = rand_dev(cell_parameters.polarity, cell_parameters.polarity_dev);
		simulation.curr_cells[id].polarity_x = cosf(M_PI * angle / 180);
		simulation.curr_cells[id].polarity_y = sinf(M_PI * angle / 180);
	}
	else {
		simulation.curr_cells[id].polarity_x = simulation.curr_cells[id].polarity_y = 0;
	}
	for (int i = 0; i < MAX_CHEMICALS; i++) {
			simulation.curr_cells[id].conc[i] = rand_dev(cell_parameters.chem_conc[i], cell_parameters.chem_conc_dev[i]);
			simulation.curr_cells[id].diff[i] = rand_dev(cell_parameters.chem_diff[i], cell_parameters.chem_diff_dev[i]);
	}
	simulation.curr_cells[id].fixed  = fixed;
	simulation.curr_cells[id].marker = false;

    return id;
}

// NOTE: this function is called only during the simulation, triggered by the evaluation of 'divide' rule
// NOTE: a child cell is added to 'next_cells' only after the current iteration is finished

static void divide_cell(CellId parent_id, CellId id, float direction, float direction_dev)
{
	float angle = atan2f(simulation.curr_cells[parent_id].polarity_y, simulation.curr_cells[parent_id].polarity_x);
	angle += M_PI * rand_dev(direction, direction_dev) / 180; // change angle by relative direction
	float dx = cosf(angle);
	float dy = sinf(angle);
	float x = simulation.curr_cells[parent_id].x + dx; // displacement == radius
	float y = simulation.curr_cells[parent_id].y + dy; // displacement == radius

	simulation.next_cells[id] = simulation.curr_cells[parent_id]; // copy everything

	simulation.next_cells[id].birth = simulation.iteration + 1; // new cells are created after current iteration ends
	simulation.next_cells[id].neighbors = 0;
	simulation.next_cells[id].x = x;
	simulation.next_cells[id].y = y;
	simulation.next_cells[id].polarity_x = dx; // polarity is set to match division direction
	simulation.next_cells[id].polarity_y = dy; // polarity is set to match division direction
	// chemical concentrations and diffusion rates are the same as parent
	simulation.next_cells[id].fixed  = false; // division always creates free (non-fixed) cells
	simulation.next_cells[id].marker = false;
}

void simulation_create_square_grid(int count_x, int count_y, float center_x, float center_y, float dev, bool fixed, bool wrap)
{
	nns_sqr_dim_x = count_x;
	nns_sqr_dim_y = count_y;
    nns_sqr_wrap = wrap;

    for (int cy = 0; cy < count_y; cy++) {
		float y = center_y + (cy - count_y / 2.0F) * 2 + 1;
    	for (int cx = 0; cx < count_x; cx++) {
    		float x = center_x + (cx - count_x / 2.0F) * 2 + 1;
    		if (dev == 0) {
    			simulation_create_cell(x, y, fixed);
    		}
    		else {
    			simulation_create_cell(x + rand_dev(-dev, dev), y + rand_dev(-dev, dev), fixed);
    		}
		}
	}
}

void simulation_create_square_circle(int count, float center_x, float center_y, float dev, bool fixed)
{
    for (int cy = 0; cy < count; cy++) {
		float y = center_y + (cy - count / 2.0F) * 2 + 1;
    	for (int cx = 0; cx < count; cx++) {
    		float x = center_x + (cx - count / 2.0F) * 2 + 1;
    		if ((cy - count / 2.0 + 0.5) * (cy - count / 2.0 + 0.5) + (cx - count / 2.0 + 0.5) * (cx - count / 2.0 + 0.5) <= count * count / 4.0) {
    			if (dev == 0) {
    				simulation_create_cell(x, y, fixed);
    			}
    			else {
    				simulation_create_cell(x + rand_dev(-dev, dev), y + rand_dev(-dev, dev), fixed);
    			}
    		}
		}
	}
}

void simulation_create_hexagonal_grid(int count_x, int count_y, float center_x, float center_y, float dev, bool fixed)
{
    for (int cy = 0; cy < count_y; cy++) {
		float y = center_y + (cy - count_y / 2.0F) * 1.7321F + 0.866F;
    	for (int cx = 0; cx < count_x; cx++) {
    		float x = center_x + (cx - count_x / 2.0F) * 2 + 1;
    	    if (cy % 2) {
    	        x += 1; // cell radius
    	    }
    		if (dev == 0) {
    			simulation_create_cell(x, y, fixed);
    		}
    		else {
    			simulation_create_cell(x + rand_dev(-dev, dev), y + rand_dev(-dev, dev), fixed);
    		}
		}
	}
}

void simulation_create_hexagonal_circle(int count, float center_x, float center_y, float dev, bool fixed)
{
    for (int cy = -count; cy < count; cy++) {
		float y = center_y + cy * 1.7321F;
    	for (int cx = -count; cx < count; cx++) {
    		float x = center_x + cx * 2;
			if (cy % 2) {
				x += 1; // cell radius
			}
    		if ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y) <= count * count) {
    			if (dev == 0) {
    				simulation_create_cell(x, y, fixed);
    			}
    			else {
    				simulation_create_cell(x + rand_dev(-dev, dev), y + rand_dev(-dev, dev), fixed);
    			}
    		}
		}
	}
}

/*-------------------------------- SET FUNCTIONS --------------------------------*/

void simulation_set_cell_concentration(CellId id, int chemical, float value, float deviation)
{
	simulation.curr_cells[id].conc[chemical] = rand_dev(value, deviation);
}

void simulation_set_cell_diffusion(CellId id, int chemical, float value, float deviation)
{
	simulation.curr_cells[id].diff[chemical] = rand_dev(value, deviation);
}

void simulation_set_cell_polarity(CellId id, float angle, float deviation)
{
	if (angle != FLT_MAX) {
		float a = rand_dev(angle, deviation);
		simulation.curr_cells[id].polarity_x = cosf(M_PI * a / 180);
		simulation.curr_cells[id].polarity_y = sinf(M_PI * a / 180);
	}
	else {
		simulation.curr_cells[id].polarity_x = simulation.curr_cells[id].polarity_y = 0;
	}
}

void simulation_set_cell_fixed(CellId id, bool fixed)
{
	simulation.curr_cells[id].fixed = fixed;
}
/*-------------------------------- RULE FUNCTIONS --------------------------------*/

void simulation_add_rule(const Rule& rule)
{
	simulation.rules.push_back(rule);

	if (rule.action == DIVIDE) {
		nns_sqr_dim_x = nns_sqr_dim_y = 0; // do not use square grid nns
	}
}

/*-------------------------------- SIMULATION FUNCTIONS --------------------------------*/

void simulation_init(NNSChoice nns_choice, bool detect_stability)
{
	switch (nns_choice) {
	case AUTO:
		if (nns_sqr_dim_x && nns_sqr_dim_y && (nns_sqr_dim_x * nns_sqr_dim_y == simulation.n_cells)) {
			nns = new NNS_SquareGrid(nns_sqr_dim_x, nns_sqr_dim_y, nns_sqr_wrap);
			std::cout << "nns: using square grid " << nns_sqr_dim_x << " x " << nns_sqr_dim_y << " wrap=" << (nns_sqr_wrap ? "true" : "false") << " (auto)\n";
		}
		else if (simulation.domain_is_packed) {
			nns = new NNS_SpatialSorting(48);
			std::cout << "nns: using spatial sorting with neighborhood m=48 (auto)\n";
		}
		else {
			nns = new NNS_KD_Tree();
			std::cout << "nns: using k-d tree (auto)\n";
		}
		break;
	case SPATIAL_SORTING:
		nns = new NNS_SpatialSorting(48);
		std::cout << "nns: using spatial sorting with neighborhood m=48\n";
		break;
	case KD_TREE:
		nns = new NNS_KD_Tree();
		std::cout << "nns: using k-d tree\n";
		break;
	}
	simulation.detect_stability = detect_stability;

	statistics.start();
	for (CellId id = CellId(0); id < simulation.n_cells; id++) {
		statistics.update(simulation.curr_cells[id], simulation.n_chemicals);
		nns->add_position(simulation.curr_cells[id].x, simulation.curr_cells[id].y, id);
	}
	statistics.finish(simulation.n_cells);

	//time_nns_setup = /*time_evaluate = time_nns_query = time_interact*/ time_calculate = time_total = 0;
}

static float get_parameter(Parameter par, float val, CellId id)
{
	if (par == CONSTANT) {
		return val;
	}
	else if (par == NEIGHBORS) {
		return (float) simulation.curr_cells[id].neighbors;
	}
	else if (par == AGE) {
		return (float) simulation.iteration - simulation.curr_cells[id].birth;
	}
	else if (par == BIRTH) {
		return (float) simulation.curr_cells[id].birth;
	}
	else if (par < MAX_CHEMICALS) {
		return simulation.curr_cells[id].conc[par];
	}
	else if (par < 2 * MAX_CHEMICALS) {
		return simulation.curr_cells[id].diff[par - MAX_CHEMICALS];
	}
	else {
		return simulation.curr_mappings[par - 2 * MAX_CHEMICALS];
	}
}

void simulation_single_step()
{
	//struct timespec start, end, t0, t1, t2; //, t3;
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	/*---------------- packed domain ----------------*/
	if (simulation.domain_is_packed) {
		float area = simulation.n_cells * simulation.domain_packed_area;

		float side = sqrt(area) - 2; // limit is for cell centers, so half-cells can extend outside the domain
		simulation.domain_xmin = - side / 2;
		simulation.domain_xmax = + side / 2;
		simulation.domain_ymin = - side / 2;
		simulation.domain_ymax = + side / 2;
	}

	/*---------------- setup phase ----------------*/

	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);

	nns->setup();

	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    //time_nns_setup += (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) * 0.000001;

	/*---------------- gather / calculation phase ----------------*/

	statistics.start();

	int n_cells = simulation.n_cells; // new cells will be processed by the next simulation step
	int n_divisions = 0; // number of cells created by division

	int n_chemicals = simulation.n_chemicals;
	int n_rules = (int) simulation.rules.size();

    float dt = simulation.time_step;

    /*---------------- iterate through all cells ----------------*/

    nns->set_start_position();
    while (nns->has_next_position()) {

    	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);

        const CellId curr_id = nns->get_current_cell_id();
        const Cell& curr_cell = simulation.curr_cells[curr_id];

        // copy current cell values as base for next cell
        Cell next_cell = curr_cell;
        next_cell.marker = false;

        int polarity_source = -1; // do not compute polarity by default, unless a rule defines a source concentration or diffusion

    	/*---------------- process rules for current cell ----------------*/

        bool logical_and = false;
        bool last_predicate = false;
        for (int r = 0; r < n_rules; r++) {
        	const Rule& rule = simulation.rules[r];

        	// evaluate iteration interval
        	if (simulation.iteration < rule.from || rule.until < simulation.iteration) {
        		if (rule.action == AND) {
        			// even if outside the interval, we must start a LOGICAL AND to affect next rule(s)
        			logical_and = true;
        			last_predicate = false;
        		}
        		// skip this rule
        		continue;
        	}

        	// evaluate predicate
        	bool is_active = false;
        	switch (rule.predicate) {
				case ALWAYS:
					is_active = true;
					break;

				case IF_EQUAL:
					is_active = (get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id) == get_parameter(rule.pr_par[1], rule.pr_val[1], curr_id));
					break;

				case IF_NOT_EQUAL:
					is_active = (get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id) != get_parameter(rule.pr_par[1], rule.pr_val[1], curr_id));
					break;

				case IF_LESS_THAN:
					is_active = (get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id) < get_parameter(rule.pr_par[1], rule.pr_val[1], curr_id));
					break;

				case IF_LESS_EQUAL:
					is_active = (get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id) <= get_parameter(rule.pr_par[1], rule.pr_val[1], curr_id));
					break;

				case IF_GREATER_THAN:
					is_active = (get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id) > get_parameter(rule.pr_par[1], rule.pr_val[1], curr_id));
					break;

				case IF_GREATER_EQUAL:
					is_active = (get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id) >= get_parameter(rule.pr_par[1], rule.pr_val[1], curr_id));
					break;

				case IF_IN_INTERVAL: {
					float v = get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id);
					is_active = (get_parameter(rule.pr_par[1], rule.pr_val[1], curr_id) <= v && v <= get_parameter(rule.pr_par[2], rule.pr_val[2], curr_id));
					break;
				}

				case PROBABILITY:
					is_active = (rand_range(0, 1) <= get_parameter(rule.pr_par[0], rule.pr_val[0], curr_id));
					break;

				default:
					std::cerr << "sim: unknown predicate in rule " << r << '\n';
					PAUSE_AND_EXIT;
        	}

        	// implement logical AND for two or more rules
        	if (logical_and) {
        		logical_and = false;
        		is_active = last_predicate && is_active;
        	}
    		if (rule.action == AND) {
    			logical_and = true;
    			last_predicate = is_active;
    			continue; // skip to next rule
    		}

        	// perform action
        	if (is_active) {
        		switch (rule.action) {
					case REACT_TU: {
						float u = curr_cell.conc[rule.ac_par[0]];
						float v = curr_cell.conc[rule.ac_par[1]];
						float s = get_parameter(rule.ac_par[2], rule.ac_val[2], curr_id);
						float alpha = get_parameter(rule.ac_par[3], rule.ac_val[3], curr_id);
						float beta = get_parameter(rule.ac_par[4], rule.ac_val[4], curr_id);

						// Turing non-linear model
						next_cell.conc[rule.ac_par[0]] += s * (alpha - u * v)    * dt;
						next_cell.conc[rule.ac_par[1]] += s * (u * v - v - beta) * dt;
						break;
					}

					case REACT_GS: {
						float u = curr_cell.conc[rule.ac_par[0]];
						float v = curr_cell.conc[rule.ac_par[1]];
						float s = get_parameter(rule.ac_par[2], rule.ac_val[2], curr_id);
						float f = get_parameter(rule.ac_par[3], rule.ac_val[3], curr_id);
						float k = get_parameter(rule.ac_par[4], rule.ac_val[4], curr_id);

						// Gray-Scott model
						next_cell.conc[rule.ac_par[0]] += s * (-u * v * v + f * (1 - u)) * dt;
						next_cell.conc[rule.ac_par[1]] += s * ( u * v * v - (f + k) * v) * dt;
						break;
					}

					case REACT_LI: {
						float u = curr_cell.conc[rule.ac_par[0]];
						float v = curr_cell.conc[rule.ac_par[1]];
						float s =  get_parameter(rule.ac_par[2], rule.ac_val[2], curr_id); // 'scale' parameter
						float au = get_parameter(rule.ac_par[3], rule.ac_val[3], curr_id);
						float bu = get_parameter(rule.ac_par[4], rule.ac_val[4], curr_id);
						float cu = get_parameter(rule.ac_par[5], rule.ac_val[5], curr_id);
						float du = get_parameter(rule.ac_par[6], rule.ac_val[6], curr_id);
						float mu = get_parameter(rule.ac_par[7], rule.ac_val[7], curr_id); // maximum synthesis amount for u
						float av = get_parameter(rule.ac_par[8], rule.ac_val[8], curr_id);
						float bv = get_parameter(rule.ac_par[9], rule.ac_val[9], curr_id);
						float cv = get_parameter(rule.ac_par[10], rule.ac_val[10], curr_id);
						float dv = get_parameter(rule.ac_par[11], rule.ac_val[11], curr_id);
						float mv = get_parameter(rule.ac_par[12], rule.ac_val[12], curr_id); // maximum synthesis amount for v

						// Turing linear model (from Kondo's 2010 simRDj applet)
						float su = au * u + bu * v + cu;
						float sv = av * u + bv * v + cv;
						if (su < 0) { su = 0; } else if (su > mu) { su = mu; }
						if (sv < 0) { sv = 0; } else if (sv > mv) { sv = mv; }
						next_cell.conc[rule.ac_par[0]] += s * (su - du * u) * dt;
						next_cell.conc[rule.ac_par[1]] += s * (sv - dv * v) * dt;
						break;
					}

					case REACT_GM1: {
						float u = curr_cell.conc[rule.ac_par[0]];
						float v = curr_cell.conc[rule.ac_par[1]];
						float s = get_parameter(rule.ac_par[2], rule.ac_val[2], curr_id);
						float a = get_parameter(rule.ac_par[3], rule.ac_val[3], curr_id);
						float b = get_parameter(rule.ac_par[4], rule.ac_val[4], curr_id);
						float c = get_parameter(rule.ac_par[5], rule.ac_val[5], curr_id);

						// Gierer-Meinhardt model (from Miyazawa's 2010 paper)
						next_cell.conc[rule.ac_par[0]] += s * (a - b * u + (u * u) / (v * (1 + c * u * u))) * dt;
						next_cell.conc[rule.ac_par[1]] += s * (u * u - v) * dt;
						break;
					}

					case REACT_CU: {
						float u = curr_cell.conc[rule.ac_par[0]];
						//float v = curr_cell.conc[rule.ac_par[1]]; // second parameter is not used
						float s = get_parameter(rule.ac_par[2], rule.ac_val[2], curr_id);
						float a = get_parameter(rule.ac_par[3], rule.ac_val[3], curr_id);
						float b = get_parameter(rule.ac_par[4], rule.ac_val[4], curr_id);
						float c = get_parameter(rule.ac_par[5], rule.ac_val[5], curr_id);

						// cubic reinforcement equation (single-reagent)
						next_cell.conc[rule.ac_par[0]] += s * (u - a) * (u - b) * (u - c) * dt;
						break;
					}

					case CHANGE: { // changes a given reagent concentration or diffusion rate
						float val = get_parameter(rule.ac_par[1], rule.ac_val[1], curr_id);
						float dev = get_parameter(rule.ac_par[2], rule.ac_val[2], curr_id);
						if (rule.ac_par[0] < MAX_CHEMICALS) {
							// concentration
							next_cell.conc[rule.ac_par[0]] += rand_dev(val, dev);
						}
						else {
							// diffusion rate
							next_cell.diff[rule.ac_par[0] - MAX_CHEMICALS] += rand_dev(val, dev);

							// we check for negative diffusion only after a change action; there is no need to test after each iteration
							if (next_cell.diff[rule.ac_par[0] - MAX_CHEMICALS] < 0) {
								next_cell.diff[rule.ac_par[0] - MAX_CHEMICALS] = 0;
							}
						}
						break;
					}

					case MAP: { // creates a mapping value
						float val = get_parameter(rule.ac_par[0], rule.ac_val[0], curr_id);
						float map;
						if (val < rule.ac_val[1]) {
							map = rule.ac_val[4];
						}
						else if (val > rule.ac_val[2]) {
							map = rule.ac_val[5];
						}
						else {
							// linearly interpolate 'val' into 'map'
							// TODO: part could be precomputed and stored in the rule itself
							map = ((val - rule.ac_val[1]) / (rule.ac_val[2] - rule.ac_val[1])) * (rule.ac_val[5] - rule.ac_val[4]) + rule.ac_val[4];
						}
						simulation.curr_mappings[rule.ac_par[3] - 2 * MAX_CHEMICALS] = map;
						break;
					}

					case POLARIZE: // orients cell based on a given concentration reagent
						polarity_source = rule.ac_par[0];
						next_cell.polarity_x = next_cell.polarity_y = 0;
						break;

					case DIVIDE: // divides cell
						if (simulation.division_limit == 0 || curr_cell.neighbors <= simulation.division_limit) {
							float dir = get_parameter(rule.ac_par[0], rule.ac_val[0], curr_id);
							float dev = get_parameter(rule.ac_par[1], rule.ac_val[1], curr_id);
							CellId child_id = simulation.new_cell();
							if (child_id != -1) {
								divide_cell(curr_id, child_id, dir, dev);
								n_divisions++;
							}
						}
						break;

					case MOVE: { // moves cell along polarity vector
						float val = get_parameter(rule.ac_par[0], rule.ac_val[0], curr_id);
						float dev = get_parameter(rule.ac_par[1], rule.ac_val[1], curr_id);
						float offset = rand_dev(val, dev);
						next_cell.x += curr_cell.polarity_x * offset;
						next_cell.y += curr_cell.polarity_y * offset;
						break;
					}

					default:
						std::cerr << "sim: unknown action in rule " << r << '\n';
						PAUSE_AND_EXIT;
        		}
			}
        }

    	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
        //time_evaluate += (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) * 0.000001;

        /*---------------- locate and interact with nearest neighbors ----------------*/

        // get all neighbors within range
        CellId *neighbor = nns->query_current_range(INFLUENCE_RANGE);
        int n_neighbors = 0;

    	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        //time_nns_gather += (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) * 0.000001;

        // iterate through all neighbors
        while ((*neighbor) != -1) {
            const CellId neig_id = (*neighbor);
            const Cell& neig_cell = simulation.curr_cells[neig_id];

            float dx = neig_cell.x - curr_cell.x;
            float dy = neig_cell.y - curr_cell.y;
            float norm = sqrtf(dx * dx + dy * dy);

        	// relocate a wrapped neighbor into a nearby position -- only occurs for wrapped square lattice
            if (norm > INFLUENCE_RANGE) {
            	if (dx > INFLUENCE_RANGE) {
            		dx = -2;
            	}
            	else if (dx < - INFLUENCE_RANGE) {
            		dx = 2;
            	}
            	if (dy > INFLUENCE_RANGE) {
            		dy = -2;
            	}
            	else if (dy < - INFLUENCE_RANGE) {
            		dy = 2;
            	}
                norm = sqrtf(dx * dx + dy * dy);
            }

            // count cell neighbors
            n_neighbors++;

            /*---------------- account diffusion from neighbors --------------*/

            for (int ch = 0; ch < n_chemicals; ch++) {
        		float diff = curr_cell.diff[ch];
        		float neig_diff = neig_cell.diff[ch];

        		// anisotropic diffusion is enabled per chemical
        		if (simulation.chemicals[ch].anisotropic) {
            		// modulate diffusion rate for current cell
            		float px = curr_cell.polarity_x;
            		float py = curr_cell.polarity_y;
            		if (px != 0 || py != 0) {
            			diff *= fabs(dx * px + dy * py) / norm;
            		}

            		// modulate diffusion rate for neighbor cell
            		px = neig_cell.polarity_x;
            		py = neig_cell.polarity_y;
            		if (px != 0 || py != 0) {
            			neig_diff *= fabs(dx * px + dy * py) / norm;
            		}
        		}

				if (diff != neig_diff) {
					// calculate combined diffusion rate
					diff = 2 * (diff * neig_diff) / (diff + neig_diff);
				}
				next_cell.conc[ch] += diff * (neig_cell.conc[ch] - curr_cell.conc[ch]) * dt;

				// this check would prevent chemical production when using a negative diffusion rate, but it is too expensive
				// if ((neig_cell.diff[ch] < 0 || curr_cell.diff[ch] < 0) && (neig_cell.conc[ch] <= 0 || curr_cell.conc[ch] <= 0))
				// { do not diffuse } else { diffuse normally }
            }

            // define polarity for this cell, calculating gradient for reference chemical concentration
            if (polarity_source != -1)
            {
            	next_cell.polarity_x += (neig_cell.conc[polarity_source] - curr_cell.conc[polarity_source]) * dx / norm;
            	next_cell.polarity_y += (neig_cell.conc[polarity_source] - curr_cell.conc[polarity_source]) * dy / norm;

            	// IDEA: to calculate vector pointing to inside of tissue; usable only for cells with 4 or less neighbors
            	// next_cell.polarity_x += dx / norm;
            	// next_cell.polarity_y += dy / norm;
            }

            // FIXME: this is a hack for finding nearest neighbors
            if (neig_id == simulation.tracked_id) {
            	next_cell.marker = true;
            }

            /*---------------- collision --------------*/

            if (curr_cell.fixed == false) {
				// VERY GOOD COLLISION: simpler, quick to stabilize and fewer holes
				if (0 < norm && norm < 2) {
					// NOTE: 2 is the sum of curr and neig radii
					// NOTE: weight factor was 0.2 -> makes possible large concentrations of cells
					// NOTE: the original collision code for distinct radii was
					// float sr = curr_cell.r + neig_cell.r;
					// float weight = 0.5 * curr_cell.r * (1 / norm) * (sr - norm) / sr;
					// next_cell.x -= weight * dx;
					// next_cell.y -= weight * dy;

					next_cell.x += (0.25F - 0.5F / norm) * dx;
					next_cell.y += (0.25F - 0.5F / norm) * dy;
				}
            }

            // go to next neighbor
            neighbor++;
        }
        next_cell.neighbors = n_neighbors;

    	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t3);
        //time_interact += (t3.tv_sec - t2.tv_sec) * 1000.0 + (t3.tv_nsec - t2.tv_nsec) * 0.000001;

        /*---------------- limit final position and concentrations --------------*/

        if      (next_cell.x < simulation.domain_xmin) { next_cell.x = simulation.domain_xmin; }
        else if (next_cell.x > simulation.domain_xmax) { next_cell.x = simulation.domain_xmax; }

        if      (next_cell.y < simulation.domain_ymin) { next_cell.y = simulation.domain_ymin; }
        else if (next_cell.y > simulation.domain_ymax) { next_cell.y = simulation.domain_ymax; }

       	for (int ch = 0; ch < n_chemicals; ch++) {
       		if      (next_cell.conc[ch] < 0)                              { next_cell.conc[ch] = 0; }
       		else if (next_cell.conc[ch] > simulation.chemicals[ch].limit) { next_cell.conc[ch] = simulation.chemicals[ch].limit; }

       		// diffusion does not need to be checked here, as it is only set by the experiment or altered by a change action
       		// if (next_cell.diff[c] < 0) { next_cell.diff[c] = 0; }
       	}

        /*---------------- normalize polarity vector --------------*/

       	if (polarity_source != -1)
       	{
       		float n = sqrtf(next_cell.polarity_x * next_cell.polarity_x + next_cell.polarity_y * next_cell.polarity_y);
       		if (n > 0.0001) {
       			next_cell.polarity_x /= n;
       			next_cell.polarity_y /= n;
       		}
       		else {
       			// previous polarity is now maintained in the absence of chemical gradient
       			next_cell.polarity_x = curr_cell.polarity_x;
       			next_cell.polarity_y = curr_cell.polarity_y;
       			// NOTE: previously it was forced to zero
       			// next_cell.polarity_x = next_cell.polarity_y = 0;
       		}
       	}

       	// IDEA: when calculating vectors pointing to inside of tissue: cells with more than 4 neighbors should not polarize
       	// if (n_neighbors > 4) {
       	//	 next_cell.polarity_x = next_cell.polarity_y = 0;
       	// }

        /*---------------- store modified cell and update statistics --------------*/

       	simulation.next_cells[curr_id] = next_cell;
       	statistics.update(next_cell, n_chemicals);
    }

	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
    //time_calculate += (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) * 0.000001;

    /*---------------- mirroring strategy #3 (average) --------------*/

    if (simulation.mirroring) {
       	for (int i = 0; i < (int) simulation.mirror_list.size(); i++) {
       		CellId primary_id   = simulation.mirror_list[i].first;
       		CellId secondary_id = simulation.mirror_list[i].second;

       		Cell& primary_cell   = simulation.next_cells[primary_id];
       		Cell& secondary_cell = simulation.next_cells[secondary_id];

       	    // TODO: what should be done with polarity_x and polarity_y?

       		for (int c = 0; c < n_chemicals; c++) {
       			float conc = (primary_cell.conc[c] + secondary_cell.conc[c]) / 2;
       			float diff = (primary_cell.diff[c] + secondary_cell.diff[c]) / 2;

       			primary_cell.conc[c] = secondary_cell.conc[c] = conc;
       			primary_cell.diff[c] = secondary_cell.diff[c] = diff;
       		}
       	}
   	}

    /*---------------- detect chemical stability --------------*/

    if (simulation.detect_stability) {
    	bool stable = true;
    	for (int i = 0; i < n_cells; i++) {
    		if (fabs(simulation.next_cells[i].conc[0] - simulation.curr_cells[i].conc[0]) >= 0.0001) {
    			stable = false;
    			break;
    		}
    	}
    	if (stable) {
    		std::cout << "sim: stability reached at " << simulation.iteration << '\n';
    		simulation.is_stable = true;
    	}
    }

    /*---------------- finalize--------------*/

    // 'next_cells' now becomes 'curr_cells'
    simulation.swap_cells();
    simulation.iteration++;

    // update NNS with new cell positions
    nns->update_all_positions(simulation.curr_cells);

    // insert new cells created by division into NNS data structure and update statistics
    for (int k = n_cells; k < n_cells + n_divisions; k++) {
    	nns->add_position(simulation.curr_cells[k].x, simulation.curr_cells[k].y, (CellId) k);
       	statistics.update(simulation.curr_cells[k], n_chemicals);
    }
    statistics.finish(n_cells + n_divisions);

	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    //time_total += (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) * 0.000001;
}

void simulation_run(int steps)
{
	for (int i = 0; i < steps; i++) {
		simulation_single_step();
		if (simulation.iteration == simulation.stop_at) {
			std::cout << "sim: stopped at " << simulation.iteration << "\n";
			simulation.is_running = false;
			break;
		} else if (simulation.detect_stability && simulation.is_stable) {
			break;
		}
	}
}

void simulation_reset()
{
	delete nns; nns = NULL;
	simulation.reset();
	cell_parameters.reset();

	nns_sqr_dim_x = nns_sqr_dim_y = 0;
	nns_sqr_wrap = false;

	rand_seed(1);
}

void simulation_done()
{
	delete nns; nns = NULL;

	//float other = time_total - time_nns_setup - time_evaluate - time_nns_gather - time_interact;
    //float other = time_total - time_nns_setup - time_calculate;

	//printf("\n");
	//printf("nns setup   %8.1f ms   %4.1f%%\n", time_nns_setup,  100 * time_nns_setup  / time_total);
	//printf("evaluate    %8.1f ms   %4.1f%%\n", time_evaluate,   100 * time_evaluate   / time_total);
	//printf("nns gather  %8.1f ms   %4.1f%%\n", time_nns_gather, 100 * time_nns_gather / time_total);
	//printf("interact    %8.1f ms   %4.1f%%\n", time_interact,   100 * time_interact   / time_total);
	//printf("calculate   %8.1f ms   %4.1f%%\n", time_calculate,  100 * time_calculate  / time_total);
	//printf("other       %8.1f ms   %4.1f%%\n", other,           100 * other           / time_total);
	//printf("total       %8.1f ms  100.0%%  iter=%d\n", time_total, simulation.iteration);

	//printf("iter/s  %7.0f\n", simulation.iteration / (time_sort + time_calc));
	//printf("iter/s  %8.1f\n", 1000 * simulation.iteration / time_total);
    //printf("\n");
}
