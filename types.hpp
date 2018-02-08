#ifndef TYPES_HPP
#define TYPES_HPP

/*-------------------------------- COMPATIBILITY --------------------------------*/

#ifdef _MSC_VER
#define PAUSE_AND_EXIT system("pause");exit(1)
#define M_PI 3.141593F
#else
#define PAUSE_AND_EXIT exit(1)
#endif

/*-------------------------------- INCLUDES --------------------------------*/

#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "strong_typedef.hpp"

/*-------------------------------- MACRO DEFINITIONS --------------------------------*/

#define VERSION "v0.55"

// 2 * cell radius * 1.1 = 2.2 // four-cell neighborhood: does not propagate adequately (von Neumann)
// 2 * cell radius * 1.5 = 3.0 // eight-cell neighborhood (Moore)

#define INFLUENCE_RANGE 3.0F

#define MAX_CELLS      20000
#define MAX_CHEMICALS  10
#define MAX_MAPPINGS   10
#define MAX_RULES      20
#define MAX_PARAMETERS 15

#ifdef __GNUC__
#  define UNUSED __attribute__((__unused__))
#else
#  define UNUSED
#endif

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

STRONG_TYPEDEF(int, CellId)

class Cell {
public:
    int   birth, neighbors;
	float x, y;
    float polarity_x, polarity_y;
    float conc[MAX_CHEMICALS];
    float diff[MAX_CHEMICALS];
    bool  fixed;
    bool  marker; // used only for neighborhood display
};

class Chemical {
public:
    std::string name;
	float       limit;
	bool        anisotropic;

	Chemical()
	{
		reset();
	}

	void reset()
	{
		limit = FLT_MAX;
		anisotropic = false;
	}
};

enum Predicate {NEVER, ALWAYS, IF_EQUAL, IF_NOT_EQUAL, IF_LESS_THAN, IF_LESS_EQUAL, IF_GREATER_THAN, IF_GREATER_EQUAL, IF_IN_INTERVAL, PROBABILITY};
enum Action    {NONE, REACT_TU, REACT_GS, REACT_LI, REACT_GM1, REACT_CU, CHANGE, MAP, POLARIZE, DIVIDE, MOVE, AND};
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

	Rule()
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
	float cell_xmin, cell_xmax;
	float cell_ymin, cell_ymax;
	float cell_nmin, cell_nmax, cell_navg;
	float chem_min[MAX_CHEMICALS], chem_max[MAX_CHEMICALS];

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
			cell_xmin = cell_xmax = cell.x;
			cell_ymin = cell_ymax = cell.y;
			sum_neighbors = cell.neighbors;
			cell_nmin = cell_nmax = (float) cell.neighbors;
		    cell_navg = 0;

			for (int c = 0; c < n_chemicals; c++) {
				chem_min[c] = chem_max[c] = cell.conc[c];
			}
			is_first = false;
		}
		else {
			if      (cell.x < cell_xmin) { cell_xmin = cell.x; }
			else if (cell.x > cell_xmax) { cell_xmax = cell.x; }

			if      (cell.y < cell_ymin) { cell_ymin = cell.y; }
			else if (cell.y > cell_ymax) { cell_ymax = cell.y; }

			if      (cell.neighbors < cell_nmin) { cell_nmin = (float) cell.neighbors; }
			else if (cell.neighbors > cell_nmax) { cell_nmax = (float) cell.neighbors; }
			sum_neighbors += cell.neighbors;

			for (int c = 0; c < n_chemicals; c++) {
				if      (cell.conc[c] < chem_min[c]) { chem_min[c] = cell.conc[c]; }
				else if (cell.conc[c] > chem_max[c]) { chem_max[c] = cell.conc[c]; }
			}
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
    Cell *curr_cells;
    Cell *next_cells;

	int n_cells;
	int n_chemicals;

	int   division_limit;
	float domain_xmin, domain_xmax;
	float domain_ymin, domain_ymax;
	bool  domain_is_packed;
	float domain_packed_area;
	bool  is_running;
	int   iteration;
	float time_step;

    Chemical chemicals[MAX_CHEMICALS];

    CellId tracked_id; // used only for neighborhood display

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
        curr_cells = new Cell[MAX_CELLS];
        next_cells = new Cell[MAX_CELLS];

        reset();
	}

    ~Simulation()
    {
        delete[] curr_cells;
        delete[] next_cells;
    }

    void reset()
    {
		n_cells = 0;
		n_chemicals = 0;
		for (int i = 0; i < MAX_CHEMICALS; i++) {
			chemicals[i].reset();
		}

		domain_xmin = domain_ymin = -500;
		domain_xmax = domain_ymax = 500;
		domain_is_packed = false;
		// 3.2 is an empirical value, between the unit circle area (3.14159) and
		// its respective circumscribed square area (4)
		domain_packed_area = 3.2F;
		// the perfect circle packing on plane would be M_PI / 0.9069F ~ 3.464
		is_running = true;
		iteration = 0;
		time_step = 1.0;
		division_limit = 0;

        tracked_id = (CellId) -1;

        rules.clear();

        mappings.clear();

        mirror_list.clear();
        mirroring = false;

		exit_at_end = false;
		snap_at.clear();
		stop_at = -1;
		texture_width = texture_height = 256;
		zoom_level = 0;

		detect_stability = false;
	    is_stable = false;
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
