#ifndef SIMULATION_HPP
#define SIMULATION_HPP

/*-------------------------------- INCLUDES --------------------------------*/

#include "nns_base.hpp"

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

enum NNSChoice {AUTO, SPATIAL_SORTING, KD_TREE};

/*-------------------------------- EXPORTED FUNCTIONS --------------------------------*/

int simulation_define_chemical(std::string name, float limit = FLT_MAX, bool anisotropic = false);

void simulation_define_division_limit(int division_limit);
void simulation_define_domain(float width, float height);
void simulation_define_time_step(float time_step);

void simulation_define_mirror_pair(CellId id1, CellId id2);

void simulation_use_chemical_concentration(int chemical, float value, float deviation = 0);
void simulation_use_chemical_diffusion(int chemical, float value, float deviation = 0);
void simulation_use_polarity(float angle, float deviation = 0);
void simulation_use_seed(int seed);

CellId simulation_create_cell(float x, float y, bool fixed = false);

void simulation_create_square_grid(int count_x, int count_y, float center_x = 0, float center_y = 0, float dev = 0, bool fixed = false, bool wrap = false);
void simulation_create_square_circle(int count, float center_x = 0, float center_y = 0, float dev = 0, bool fixed = false);
void simulation_create_hexagonal_grid(int count_x, int count_y, float center_x = 0, float center_y = 0, float dev = 0, bool fixed = false);
void simulation_create_hexagonal_circle(int count, float center_x = 0, float center_y = 0, float dev = 0, bool fixed = false);

void simulation_set_cell_concentration(CellId id, int chemical, float value, float deviation = 0);
void simulation_set_cell_diffusion(CellId id, int chemical, float value, float deviation = 0);
void simulation_set_cell_polarity(CellId id, float angle, float deviation = 0);
void simulation_set_cell_fixed(CellId id, bool fixed);

void simulation_add_rule(const Rule& rule);

void simulation_init(NNSChoice nns_choice = AUTO, bool detect_stability = false);
void simulation_run(int steps);
void simulation_reset();
void simulation_done();

/*-------------------------------- EXPORTED VARIABLES --------------------------------*/

extern NNS *nns;
extern Simulation simulation;
extern Statistics statistics;

//extern struct timespec time_start;
//extern float time_init, time_sort, time_calc, time_draw;

#endif // SIMULATION_HPP
