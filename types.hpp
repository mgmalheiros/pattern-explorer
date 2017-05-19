#ifndef TYPES_HPP
#define TYPES_HPP

/*-------------------------------- INCLUDES --------------------------------*/

#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "strong_typedef.hpp"

/*-------------------------------- MACRO DEFINITIONS --------------------------------*/

// 2 * cell radius * 1.1 = 2.2 // four-cell neighborhood: does not propagate adequately (von Neumann)
// 2 * cell radius * 1.5 = 3.0 // eight-cell neighborhood (Moore)

#define INFLUENCE_RANGE 3.0

#define MAX_CELLS      20000
#define MAX_CHEMICALS  10
#define MAX_MAPPINGS   10
#define MAX_RULES      20
#define MAX_PARAMETERS 6

//#define NNS_PRECISION

#ifdef __GNUC__
#  define UNUSED __attribute__((__unused__))
#else
#  define UNUSED
#endif

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

STRONG_TYPEDEF(int, CellId);

class Cell {
public:
    int   birth, neighbors;
	float x, y;
    float polarity_x, polarity_y;
    float conc[MAX_CHEMICALS];
    float diff[MAX_CHEMICALS];
    bool  fixed;
    bool  marker; // TODO: needed only for neighborhood display
#ifdef NNS_PRECISION
    int error;
#endif // NNS_PRECISION
};

class Chemical {
public:
    std::string name;
	float       limit;
	bool        anisotropic;

	Chemical ()
	{
		limit = FLT_MAX;
		anisotropic = false;
	}
};

enum Predicate {NEVER, ALWAYS, IF_EQUAL, IF_NOT_EQUAL, IF_LESS_THAN, IF_LESS_EQUAL, IF_GREATER_THAN, IF_GREATER_EQUAL, IF_IN_INTERVAL, PROBABILITY};
enum Action    {NONE, REACT_GS, REACT_TU, REACT_LI, REACT_CU, CHANGE, MAP, POLARIZE, DIVIDE, MOVE, AND};
enum Parameter {CONSTANT = -1, NEIGHBORS = -2, AGE = -3, BIRTH = -4};

class Rule {
public:
	int from;
	int until;

	Predicate predicate;
	Parameter pr_par[MAX_PARAMETERS];
	float     pr_val[MAX_PARAMETERS];

	Action    action;
	Parameter ac_par[MAX_PARAMETERS];
	float     ac_val[MAX_PARAMETERS];

	Rule ()
	{
		from = 0;
		until = INT_MAX;
		predicate = NEVER;
		action = NONE;
		for (int i = 0; i < MAX_PARAMETERS; i++) {
			pr_par[i] = ac_par[i] = CONSTANT;
			pr_val[i] = ac_val[i] = 0;
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Rule& r)
	{
		os << r.predicate << " [";
		for (int i = 0; i < MAX_PARAMETERS; i++) {
			if (r.pr_par[i] == CONSTANT) {
				os << r.pr_val[i] << ' ';
			}
			else {
				os << '#' << r.pr_par[i] << ' ';
			}
		}
		os << "] " << r.action << " [";
		for (int i = 0; i < MAX_PARAMETERS; i++) {
			if (r.ac_par[i] == CONSTANT) {
				os << r.ac_val[i] << ' ';
			}
			else {
				os << '#' << r.ac_par[i] << ' ';
			}
		}
		return os << ']';
	}
};

class Statistics {
public:
	bool is_first;
	//int last_birth;
	float cell_xmin, cell_xmax;
	float cell_ymin, cell_ymax;
	float cell_nmin, cell_nmax, cell_navg;
	float chem_min[MAX_CHEMICALS], chem_max[MAX_CHEMICALS];
#ifdef NNS_PRECISION
	float error_max;
#endif // NNS_PRECISION

	Statistics()
	{
		start();
	}

	void start()
	{
		is_first = true;
	}

	void update(const Cell& cell, int n_chemicals)
	{
		if (is_first) {
			//last_birth = cell.birth;
			cell_xmin = cell_xmax = cell.x;
			cell_ymin = cell_ymax = cell.y;
			cell_nmin = cell_nmax = sum_neighbors = cell.neighbors;
		    cell_navg = 0;
#ifdef NNS_PRECISION
		    error_max = 0;
#endif // NNS_PRECISION

			for (int c = 0; c < n_chemicals; c++) {
				chem_min[c] = chem_max[c] = cell.conc[c];
			}
			is_first = false;
		}
		else {
			//if (cell.birth > last_birth) { last_birth = cell.birth; }

			if      (cell.x < cell_xmin) { cell_xmin = cell.x; }
			else if (cell.x > cell_xmax) { cell_xmax = cell.x; }

			if      (cell.y < cell_ymin) { cell_ymin = cell.y; }
			else if (cell.y > cell_ymax) { cell_ymax = cell.y; }

			if      (cell.neighbors < cell_nmin) { cell_nmin = cell.neighbors; }
			else if (cell.neighbors > cell_nmax) { cell_nmax = cell.neighbors; }
			sum_neighbors += cell.neighbors;

			for (int c = 0; c < n_chemicals; c++) {
				if      (cell.conc[c] < chem_min[c]) { chem_min[c] = cell.conc[c]; }
				else if (cell.conc[c] > chem_max[c]) { chem_max[c] = cell.conc[c]; }
			}

#ifdef NNS_PRECISION
			if (cell.error > error_max) { error_max = cell.error; }
#endif // NNS_PRECISION
		}
	}

	void finish(int n_cells)
	{
		cell_navg = (float) sum_neighbors / n_cells;
	}

private:
	int sum_neighbors;
};

class Simulation {
public:
	int n_cells;
	int n_chemicals;

	int   division_limit;
	float domain_xmin, domain_xmax;
	float domain_ymin, domain_ymax;
	bool  domain_is_packed;
	float domain_packed_factor;
	bool  is_running;
	int   iteration;
	float time_step;

    Cell *curr_cells;
    Cell *next_cells;

    Chemical chemicals[MAX_CHEMICALS];

    CellId tracked_id; // TODO: needed only for neighborhood display

    std::vector<Rule> rules;

    std::vector<std::string> mappings;
    float curr_mappings[MAX_MAPPINGS];

    std::vector<std::pair<CellId,CellId> > mirror_list;
    bool mirroring;

    bool exit_at_end;
    std::vector<int> snap_at;
	int   stop_at;
	int   texture_width, texture_height;
	float zoom_level;

    bool detect_stability;
    bool is_stable;

public:
	Simulation()
	{
		n_cells = 0;
		n_chemicals = 0;

		domain_xmin = domain_ymin = -500;
		domain_xmax = domain_ymax = 500;
		domain_is_packed = false;
		domain_packed_factor = 3.2; // empirical, between unit circle area (3.14159) and square area (4)
		is_running = true;
		iteration = 0;
		time_step = 1.0;
		division_limit = 0;

        curr_cells = new Cell[MAX_CELLS];
        next_cells = new Cell[MAX_CELLS];

        tracked_id = (CellId) -1;

        mirroring = false;

		exit_at_end = false;
		stop_at = -1;
		texture_width = texture_height = 256;
		zoom_level = 0;

		detect_stability = false;
	    is_stable = false;
	}

    ~Simulation()
    {
        delete[] curr_cells;
        delete[] next_cells;
    }

    CellId new_cell()
    {
    	if (n_cells >= MAX_CELLS) {
    		std::cout << "error: no more cells available (max " << MAX_CELLS << ")\n";
    		return (CellId) -1;
    	}
    	CellId id = (CellId) n_cells;
    	n_cells++;
    	return id;
    }

    int new_chemical()
    {
    	if (n_chemicals >= MAX_CHEMICALS) {
    		std::cout << "error: no more chemicals available (max " << MAX_CHEMICALS << ")\n";
    		return -1;
    	}
    	int ch = n_chemicals;
    	n_chemicals++;
    	return ch;
    }

    void swap_cells()
    {
        Cell *temp = curr_cells;
        curr_cells = next_cells;
        next_cells = temp;
    }
};

#endif // TYPES_HPP
