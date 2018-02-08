/*-------------------------------- INCLUDES --------------------------------*/

#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

#include "colormap.hpp"
#include "simulation.hpp"

#include "parser.hpp"

/*-------------------------------- LOCAL VARIABLES --------------------------------*/

static const char *file_name = NULL;

static std::vector<int> mirror_seq_upper[26];
static std::vector<int> mirror_seq_lower[26];

/*-------------------------------- LOCAL UTILITY FUNCTIONS --------------------------------*/

static std::string trim(std::string str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

static void error(std::string message, int line_num = 0)
{
	std::cerr << "error: " + message;
	if (line_num) {
		std::cerr << " in line " << line_num << '\n';
	}
	else {
		std::cerr << '\n';
	}
	PAUSE_AND_EXIT;
}

static int find_chemical(std::string name)
{
	for (int ch = 0; ch < simulation.n_chemicals; ch++) {
		if (simulation.chemicals[ch].name == name) {
			return ch;
		}
	}
	return -1;
}

static int add_mapping(std::string name, int line_num)
{
	if (find_chemical(name) != -1) {
		error("there is already a chemical with name " + name, line_num);
	}
	int i;
	for (i = 0; i < (int) simulation.mappings.size(); i++) {
		if (simulation.mappings[i] == name) {
			return i;
		}
	}
	simulation.mappings.push_back(name);
	return i;
}

static int find_mapping(std::string name)
{
	for (int i = 0; i < (int) simulation.mappings.size(); i++) {
		if (simulation.mappings[i] == name) {
			return i;
		}
	}
	return -1;
}

static void get_parameter(std::stringstream& ss, Parameter& par, float& val, int line_num)
{
	std::string word;
	ss >> word;
	if (word == "NEIGHBORS") {
		par = NEIGHBORS;
		return;
	}
	else if (word == "AGE") {
		par = AGE;
		return;
	}
	else if (word == "BIRTH") {
		par = BIRTH;
		return;
	}

	// check for a float
	char* end;
	float f = (float) strtod(word.c_str(), &end);
	if (word.c_str() != end) {
		par = CONSTANT;
		val = f;
		return;
	}

	// check for a chemical
	int ch = find_chemical(word);
	if (ch != -1) {
		ss >> word;
		if (word == "conc") {
			par = (Parameter) ch;
			return;
		}
		else if (word == "diff") {
			par = (Parameter) (ch + MAX_CHEMICALS);
			return;
		}
		else {
			error("expected 'conc' or 'diff' for chemical", line_num);
		}
	}

	// check for a mapping
	int i = find_mapping(word);
	if (i != -1) {
		par = (Parameter) (i + 2 * MAX_CHEMICALS);
	}
	else {
		error("unknown chemical or mapping name " + word, line_num);
	}
}

static void create_shape(std::string shape, float center_x, float center_y, bool fixed)
{
	// in case the experiment is reloaded
	for (int i = 0; i < 26; i++) {
		mirror_seq_upper[i].clear();
		mirror_seq_lower[i].clear();
	}

	std::ifstream file(shape.c_str());
	if (! file.is_open()) {
		error("cannot open shape file '" + shape + "'");
	}

	// get shape bounding box
	unsigned int count_x = 0, count_y = 0;
	std::string line;
	while (std::getline(file, line)) {
		if (line.length() > count_x) {
			count_x = (unsigned int) line.length();
		}
		count_y++;
	}
	std::cout << "shape: read " << count_x << " by " << count_y << " characters\n";

	// create cells marked by alphabetic characters
	int cy = 0;
	file.clear();
    file.seekg(0, file.beg);
	while (std::getline(file, line)) {
		float y = center_y + (count_y / 2.0F - cy) * 2;
		int len = (int) line.length();
		for (int cx = 0; cx < len; cx++) {
			if (isalpha(line[cx])) {
	    		float x = center_x + (cx - count_x / 2.0F) * 2;
	    		CellId id = simulation_create_cell(x, y, fixed);

	    		// standard cell
	    		if (line[cx] == 'o' || line[cx] == 'O') {
	    			continue;
	    		}

	    		// mirror cell
	    		if (isupper(line[cx])) {
	    			mirror_seq_upper[line[cx] - 'A'].push_back(id);
	    		}
	    		else if (islower(line[cx])) {
	    			mirror_seq_lower[line[cx] - 'a'].push_back(id);
	    		}
			}
		}
		cy++;
	}

	// insert mirror pairs from matching alphabetic sequences
	for (int i = 0; i < 26; i++) {
		int reverse = -1;
		while (mirror_seq_upper[i].size() && mirror_seq_lower[i].size())
		{
			if (reverse == -1) {
				if (mirror_seq_lower[i].front() < mirror_seq_upper[i].front()) {
					std::cout << "shape: reversed matching between '" << char('A' + i) << "' and '" << char('a' + i) << "'\n";
					reverse = 1;
				}
				else {
					reverse = 0;
				}
			}

			// if a lower case character precedes an upper case character, reverse the matching order
			CellId id1, id2;
			if (reverse == 0) {
				id1 = (CellId) mirror_seq_upper[i].front();
				id2 = (CellId) mirror_seq_lower[i].front();
				mirror_seq_upper[i].erase(mirror_seq_upper[i].begin());
				mirror_seq_lower[i].erase(mirror_seq_lower[i].begin());
			}
			else {
				id1 = (CellId) mirror_seq_upper[i].front();
				id2 = (CellId) mirror_seq_lower[i].back();
				mirror_seq_upper[i].erase(mirror_seq_upper[i].begin());
				mirror_seq_lower[i].pop_back();
			}
			simulation_define_mirror_pair(id1, id2);
		}
		if (mirror_seq_upper[i].size()) {
			std::cout << "shape: found " << mirror_seq_upper[i].size() << " extra cell(s) for '" << char('A' + i) << "'\n";
			for (unsigned int j = 0; j < mirror_seq_upper[i].size(); j++) {
				std::cout << mirror_seq_upper[i][j] << ' ';
			}
			std::cout << '\n';
		}
		if (mirror_seq_lower[i].size()) {
			std::cout << "shape: found " << mirror_seq_lower[i].size() << " extra cell(s) for '" << char('a' + i) << "'\n";
		}
	}
}

/*-------------------------------- PARSER FUNCTIONS --------------------------------*/

void parser_init(const char *name)
{
	file_name = name;
}

void parser_load_pattern()
{
	std::ifstream file(file_name);
	if (! file.is_open()) {
		error("cannot open pattern file '" + std::string(file_name) + "'");
	}

	int n = 0;
	std::string line, word;
	while (std::getline(file, line)) {
	    n++;

		line = trim(line);
	    if (line.empty() || (line[0] == '/' && line[1] == '/')) {
	    	// skip comments and blank lines
	    	continue;
	    }

	    std::stringstream ss(line);
	    ss >> word;
	    if (word == "define") {
	    	/*---------------- define commands ----------------*/
	    	ss >> word;
		    if (word == "chemical") {
		    	std::string name;
		    	ss >> name;
		    	if (find_chemical(name) != -1) {
		    		error("chemical " + name + " already defined", n);
		    	}
		    	float limit = FLT_MAX;
		    	bool anisotropic = false;
		    	ss >> word;
		    	if (word == "limit") {
		    		ss >> limit;
		    		ss >> word;
		    	}
		    	if (word == "anisotropic") {
		    		anisotropic = true;
		    	}
		    	simulation_define_chemical(name, limit, anisotropic);
		    }
		    else if (word == "division_limit") {
		    	int division_limit;
		    	ss >> division_limit;
		    	simulation_define_division_limit(division_limit);
		    }
		    else if (word == "domain") {
		    	ss >> word;
		    	if (word == "packed") {
		    		simulation.domain_is_packed = true;
			    	ss >> word;
			    	if (word == "area") {
			    		float area;
			    		ss >> area;
			    		simulation.domain_packed_area = area;
			    	}
		    	}
		    	else {
		    		float width, height;
		    		std::stringstream(word) >> width;
		    		ss >> height;
		    		simulation_define_domain(width, height);
		    	}
		    }
		    else if (word == "time_step") {
		    	float time_step;
		    	ss >> time_step;
		    	simulation_define_time_step(time_step);
		    }
	    	else {
	    		error("unknown command " + word, n);
	    	}
	    }
	    else if (word == "use") {
	    	/*---------------- use commands ----------------*/
	    	ss >> word;
		    if (word == "chemical") {
		    	ss >> word;
		    	int ch = find_chemical(word);
		    	if (ch == -1) {
		    		error("unknown chemical " + word, n);
		    	}
		    	ss >> word;
		    	if (word == "conc") {
		    		float value = 0, deviation = 0;
		    		ss >> value;
		    		ss >> word;
		    		if (word == "dev") {
		    			ss >> deviation;
		    			ss >> word;
		    		}
		    		simulation_use_chemical_concentration(ch, value, deviation);
		    	}
		    	if (word == "diff") {
		    		float value = 0, deviation = 0;
		    		ss >> value;
		    		ss >> word;
		    		if (word == "dev") {
		    			ss >> deviation;
		    		}
		    		simulation_use_chemical_diffusion(ch, value, deviation);
		    	}
		    }
		    else if (word == "polarity") {
		    	ss >> word;
		    	if (word == "none") {
		    		simulation_use_polarity(FLT_MAX, 0);
		    	}
		    	else {
		    		float angle = (float) strtod(word.c_str(), NULL);
		    		float deviation = 0;
		    		ss >> word;
		    		if (word == "dev") {
		    			ss >> deviation;
		    		}
		    		simulation_use_polarity(angle, deviation);
		    	}
		    }
		    else if (word == "seed") {
		    	int seed;
		    	ss >> seed;
		    	simulation_use_seed(seed);
		    }
		    else {
		    	error("unknown command " + word, n);
		    }
	    }
	    else if (word == "create") {
	    	/*---------------- create commands ----------------*/
	    	ss >> word;
		    if (word == "cell") {
		    	float x, y;
		    	bool fixed = false;
		    	ss >> x;
		    	ss >> y;
	    		ss >> word;
				if (word == "fixed") {
					fixed = true;
				}
				simulation_create_cell(x, y, fixed);
		    }
		    else if (word == "sqr_grid") {
				int count_x = 0, count_y = 0;
				float x = 0, y = 0, dev = 0;
		    	bool fixed = false, wrap = false;
		    	ss >> count_x;
		    	ss >> count_y;
		    	if (count_x == 0 || count_y == 0) {
		    		error("missing or invalid grid dimensions", n);
		    	}
		    	ss >> word;
		    	if (word == "at") {
		    		ss >> x;
		    		ss >> y;
		    		ss >> word;
		    	}
		    	if (word == "dev") {
		    		ss >> dev;
		    		ss >> word;
		    	}
		    	if (word == "fixed") {
		    		fixed = true;
		    		ss >> word;
		    	}
		    	if (word == "wrap") {
		    		wrap = true;
		    	}
		    	simulation_create_square_grid(count_x, count_y, x, y, dev, fixed, wrap);
		    }
		    else if (word == "sqr_circle") {
				int count = 0;
				float x = 0, y = 0, dev = 0;
		    	bool fixed = false;
		    	ss >> count;
		    	if (count == 0) {
		    		error("missing or invalid circle diameter", n);
		    	}
		    	ss >> word;
		    	if (word == "at") {
		    		ss >> x;
		    		ss >> y;
		    		ss >> word;
		    	}
		    	if (word == "dev") {
		    		ss >> dev;
		    		ss >> word;
		    	}
		    	if (word == "fixed") {
		    		fixed = true;
		    	}
		    	simulation_create_square_circle(count, x, y, dev, fixed);
		    }
		    else if (word == "hex_grid") {
				int count_x = 0, count_y = 0;
				float x = 0, y = 0, dev = 0;
		    	bool fixed = false;
		    	ss >> count_x;
		    	ss >> count_y;
		    	if (count_x == 0 || count_y == 0) {
		    		error("missing or invalid grid dimensions", n);
		    	}
		    	ss >> word;
		    	if (word == "at") {
		    		ss >> x;
		    		ss >> y;
		    		ss >> word;
		    	}
		    	if (word == "dev") {
		    		ss >> dev;
		    		ss >> word;
		    	}
		    	if (word == "fixed") {
		    		fixed = true;
		    	}
		    	simulation_create_hexagonal_grid(count_x, count_y, x, y, dev, fixed);
		    }
		    else if (word == "hex_circle") {
				int count = 0;
				float x = 0, y = 0, dev = 0;
		    	bool fixed = false;
		    	ss >> count;
		    	if (count == 0) {
		    		error("missing or invalid circle diameter", n);
		    	}
		    	ss >> word;
		    	if (word == "at") {
		    		ss >> x;
		    		ss >> y;
		    		ss >> word;
		    	}
		    	if (word == "dev") {
		    		ss >> dev;
		    		ss >> word;
		    	}
		    	if (word == "fixed") {
		    		fixed = true;
		    	}
		    	simulation_create_hexagonal_circle(count, x, y, dev, fixed);
		    }
		    else if (word == "shape") {
		    	float x = 0, y = 0;
		    	bool fixed = false;
		    	std::string shape;
		    	ss >> shape;
		    	if (shape[0] != '"'  || shape[shape.length() - 1] != '"') {
		    		error("shape file name must be delimited by double quotes", n);
		    	}
		    	shape = shape.substr(1, shape.length() - 2);
		    	ss >> word;
		    	if (word == "at") {
		    		ss >> x;
		    		ss >> y;
		    		ss >> word;
		    	}
		    	if (word == "fixed") {
		    		fixed = true;
		    	}
		    	create_shape(shape, x, y, fixed);
		    }
	    	else {
	    		error("unknown command " + word, n);
	    	}
	    }
	    else if (word == "set") {
	    	/*---------------- set commands ----------------*/
	    	ss >> word;
		    if (word == "cell") {
		    	CellId id;
		    	ss >> id;
		    	if (id < 0 || id >= simulation.n_cells) {
		    		error("invalid cell id", n);
		    	}
		    	ss >> word;
		    	if (word == "chemical") {
			    	ss >> word;
			    	int ch = find_chemical(word);
			    	if (ch == -1) {
			    		error("unknown chemical " + word, n);
			    	}
			    	ss >> word;
			    	if (word != "conc" && word != "diff") {
			    		error("expecting either 'conc' or 'diff' keywords, found '" + word + "' instead", n);
			    	}
			    	if (word == "conc") {
			    		float value = 0, dev = 0;
			    		ss >> value;
			    		ss >> word;
			    		if (word == "dev") {
			    			ss >> dev;
			    			ss >> word;
			    		}
			    		simulation_set_cell_concentration(id, ch, value, dev);
			    	}
			    	if (word == "diff") {
			    		float value = 0, dev = 0;
			    		ss >> value;
			    		ss >> word;
			    		if (word == "dev") {
			    			ss >> dev;
			    			ss >> word;
			    		}
			    		simulation_set_cell_diffusion(id, ch, value, dev);
			    	}
		    	}
			    if (word == "polarity") {
			    	ss >> word;
			    	if (word == "none") {
			    		ss >> word;
			    		simulation_set_cell_polarity(id, FLT_MAX, 0);
			    	}
			    	else {
			    		float angle = (float) strtod(word.c_str(), NULL);
			    		float deviation = 0;
			    		ss >> word;
			    		if (word == "dev") {
			    			ss >> deviation;
			    			ss >> word;
			    		}
			    		simulation_set_cell_polarity(id, angle, deviation);
			    	}
			    }
		    	if (word == "fixed") {
		    		simulation_set_cell_fixed(id, true);
		    	}
		    }
		    else if (word == "cells") {
		    	CellId id1, id2;
		    	ss >> id1;
		    	if (id1 < 0 || id1 >= simulation.n_cells) {
		    		error("invalid cell id", n);
		    	}
		    	ss >> word;
		    	if (word != "to") {
		    		error("missing 'to' command, found '" + word + "' instead", n);
		    	}
		    	ss >> id2;
		    	if (id2 < 0 || id2 >= simulation.n_cells) {
		    		error("invalid cell id", n);
		    	}
		    	ss >> word;
		    	if (word == "chemical") {
		    		ss >> word;
		    		int ch = find_chemical(word);
		    		if (ch == -1) {
		    			error("unknown chemical " + word, n);
		    		}
		    		ss >> word;
			    	if (word != "conc" && word != "diff") {
			    		error("expecting either 'conc' or 'diff' keywords, found '" + word + "' instead", n);
			    	}
		    		if (word == "conc") {
		    			float value = 0, dev = 0;
		    			ss >> value;
		    			ss >> word;
		    			if (word == "dev") {
		    				ss >> dev;
		    				ss >> word;
		    			}
		    			for (CellId id = id1; id <= id2; id++) {
		    				simulation_set_cell_concentration(id, ch, value, dev);
		    			}
		    		}
		    		if (word == "diff") {
		    			float value = 0, dev = 0;
		    			ss >> value;
		    			ss >> word;
		    			if (word == "dev") {
		    				ss >> dev;
		    				ss >> word;
		    			}
		    			for (CellId id = id1; id <= id2; id++) {
		    				simulation_set_cell_diffusion(id, ch, value, dev);
		    			}
		    		}
		    	}
			    if (word == "polarity") {
			    	ss >> word;
			    	if (word == "none") {
			    		ss >> word;
			    		for (CellId id = id1; id <= id2; id++) {
			    			simulation_set_cell_polarity(id, FLT_MAX, 0);
			    		}
			    	}
			    	else {
			    		float angle = (float) strtod(word.c_str(), NULL);
			    		float deviation = 0;
			    		ss >> word;
			    		if (word == "dev") {
			    			ss >> deviation;
			    			ss >> word;
			    		}
			    		for (CellId id = id1; id <= id2; id++) {
			    			simulation_set_cell_polarity(id, angle, deviation);
			    		}
			    	}
			    }
		    	if (word == "fixed") {
		    		for (CellId id = id1; id <= id2; id++) {
		    			simulation_set_cell_fixed(id, true);
		    		}
		    	}
		    }
		    else {
		    	error("unknown command " + word, n);
		    }
	    }
	    else if (word == "rule") {
	    	Rule rule;
	    	ss >> word;
	    	/*---------------- rule commands: time interval ----------------*/
		    if (word == "from") {
		    	ss >> rule.from;
		    	ss >> word;
		    }
		    if (word == "until") {
		    	ss >> rule.until;
		    	ss >> word;
		    }
	    	/*---------------- rule commands: predicate ----------------*/
		    if (word == "always") {
		    	rule.predicate = ALWAYS;
		    }
		    else if (word == "if") {
		    	// extract first parameter
		    	get_parameter(ss, rule.pr_par[0], rule.pr_val[0], n);

		    	// extract comparison operator and other parameter(s)
		    	ss >> word;
		    	if (word == "==") {
		    		rule.predicate = IF_EQUAL;
			    	get_parameter(ss, rule.pr_par[1], rule.pr_val[1], n);
		    	}
		    	else if (word == "!=") {
		    		rule.predicate = IF_NOT_EQUAL;
			    	get_parameter(ss, rule.pr_par[1], rule.pr_val[1], n);
		    	}
		    	else if (word == "<") {
		    		rule.predicate = IF_LESS_THAN;
			    	get_parameter(ss, rule.pr_par[1], rule.pr_val[1], n);
		    	}
		    	else if (word == "<=") {
		    		rule.predicate = IF_LESS_EQUAL;
			    	get_parameter(ss, rule.pr_par[1], rule.pr_val[1], n);
		    	}
		    	else if (word == ">") {
		    		rule.predicate = IF_GREATER_THAN;
			    	get_parameter(ss, rule.pr_par[1], rule.pr_val[1], n);
		    	}
		    	else if (word == ">=") {
		    		rule.predicate = IF_GREATER_EQUAL;
			    	get_parameter(ss, rule.pr_par[1], rule.pr_val[1], n);
		    	}
		    	else if (word == "in") {
		    		rule.predicate = IF_IN_INTERVAL;
			    	get_parameter(ss, rule.pr_par[1], rule.pr_val[1], n);
			    	get_parameter(ss, rule.pr_par[2], rule.pr_val[2], n);
		    	}
	    		else {
	    			error("unknown comparison operator " + word, n);
	    		}
		    }
		    else if (word == "probability") {
		    	rule.predicate = PROBABILITY;
		    	get_parameter(ss, rule.pr_par[0], rule.pr_val[0], n);
		    }
    		else {
    			error("unknown predicate " + word, n);
    		}
	    	/*---------------- rule commands: actions ----------------*/
	    	ss >> word;
		    if (word == "react") {
		    	ss >> word;
		    	int ch = find_chemical(word);
		    	if (ch == -1) {
		    		error("unknown chemical " + word, n);
		    	}
		    	rule.ac_par[0] = (Parameter) ch;
		    	ss >> word;
		    	ch = find_chemical(word);
		    	if (ch == -1) {
		    		error("unknown chemical " + word, n);
		    	}
		    	rule.ac_par[1] = (Parameter) ch;
		    	ss >> word;
		    	if (word == "scale") {
			    	get_parameter(ss, rule.ac_par[2], rule.ac_val[2], n);
			    	ss >> word;
		    	}
		    	else {
		    		rule.ac_par[2] = CONSTANT;
		    		rule.ac_val[2] = 1;
		    	}
		    	if (word == "turing") {
		    		rule.action = REACT_TU;
			    	ss >> word;
				    if (word == "alpha") {
				    	get_parameter(ss, rule.ac_par[3], rule.ac_val[3], n);
				    }
				    else {
				    	error("parameter 'alpha' expected", n);
				    }
			    	ss >> word;
				    if (word == "beta") {
				    	get_parameter(ss, rule.ac_par[4], rule.ac_val[4], n);
				    }
				    else {
				    	error("parameter 'beta' expected", n);
				    }
		    	}
		    	else if (word == "gray-scott") {
		    		rule.action = REACT_GS;
			    	ss >> word;
				    if (word == "f") {
				    	get_parameter(ss, rule.ac_par[3], rule.ac_val[3], n);
				    }
				    else {
				    	error("parameter 'f' expected", n);
				    }
			    	ss >> word;
				    if (word == "k") {
				    	get_parameter(ss, rule.ac_par[4], rule.ac_val[4], n);
				    }
				    else {
				    	error("parameter 'k' expected", n);
				    }
		    	}
		    	else if (word == "linear") {
		    		rule.action = REACT_LI;
			    	ss >> word;
				    if (word == "au") {
				    	get_parameter(ss, rule.ac_par[3], rule.ac_val[3], n);
				    }
				    else {
				    	error("parameter 'au' expected", n);
				    }
			    	ss >> word;
				    if (word == "bu") {
				    	get_parameter(ss, rule.ac_par[4], rule.ac_val[4], n);
				    }
				    else {
				    	error("parameter 'bu' expected", n);
				    }
			    	ss >> word;
				    if (word == "cu") {
				    	get_parameter(ss, rule.ac_par[5], rule.ac_val[5], n);
				    }
				    else {
				    	error("parameter 'cu' expected", n);
				    }
			    	ss >> word;
				    if (word == "du") {
				    	get_parameter(ss, rule.ac_par[6], rule.ac_val[6], n);
				    }
				    else {
				    	error("parameter 'du' expected", n);
				    }
			    	ss >> word;
				    if (word == "mu") {
				    	get_parameter(ss, rule.ac_par[7], rule.ac_val[7], n);
				    }
				    else {
				    	error("parameter 'mu' expected", n);
				    }
			    	ss >> word;
				    if (word == "av") {
				    	get_parameter(ss, rule.ac_par[8], rule.ac_val[8], n);
				    }
				    else {
				    	error("parameter 'av' expected", n);
				    }
			    	ss >> word;
				    if (word == "bv") {
				    	get_parameter(ss, rule.ac_par[9], rule.ac_val[9], n);
				    }
				    else {
				    	error("parameter 'bv' expected", n);
				    }
			    	ss >> word;
				    if (word == "cv") {
				    	get_parameter(ss, rule.ac_par[10], rule.ac_val[10], n);
				    }
				    else {
				    	error("parameter 'cv' expected", n);
				    }
			    	ss >> word;
				    if (word == "dv") {
				    	get_parameter(ss, rule.ac_par[11], rule.ac_val[11], n);
				    }
				    else {
				    	error("parameter 'dv' expected", n);
				    }
			    	ss >> word;
				    if (word == "mv") {
				    	get_parameter(ss, rule.ac_par[12], rule.ac_val[12], n);
				    }
				    else {
				    	error("parameter 'mv' expected", n);
				    }
		    	}
		    	else if (word == "gm1") {
		    		rule.action = REACT_GM1;
			    	ss >> word;
				    if (word == "a") {
				    	get_parameter(ss, rule.ac_par[3], rule.ac_val[3], n);
				    }
				    else {
				    	error("parameter 'a' expected", n);
				    }
			    	ss >> word;
				    if (word == "b") {
				    	get_parameter(ss, rule.ac_par[4], rule.ac_val[4], n);
				    }
				    else {
				    	error("parameter 'b' expected", n);
				    }
			    	ss >> word;
				    if (word == "c") {
				    	get_parameter(ss, rule.ac_par[5], rule.ac_val[5], n);
				    }
				    else {
				    	error("parameter 'c' expected", n);
				    }
		    	}
		    	else if (word == "cubic") {
		    		rule.action = REACT_CU;
			    	ss >> word;
				    if (word == "a") {
				    	get_parameter(ss, rule.ac_par[3], rule.ac_val[3], n);
				    }
				    else {
				    	error("parameter 'a' expected", n);
				    }
			    	ss >> word;
				    if (word == "b") {
				    	get_parameter(ss, rule.ac_par[4], rule.ac_val[4], n);
				    }
				    else {
				    	error("parameter 'b' expected", n);
				    }
			    	ss >> word;
				    if (word == "c") {
				    	get_parameter(ss, rule.ac_par[5], rule.ac_val[5], n);
				    }
				    else {
				    	error("parameter 'c' expected", n);
				    }
		    	}
	    		else {
	    			error("unknown reaction model " + word, n);
	    		}
		    }
		    else if (word == "change") {
		    	rule.action = CHANGE;

		    	// target parameter
		    	get_parameter(ss, rule.ac_par[0], rule.ac_val[0], n);
		    	if (rule.ac_par[0] == CONSTANT) {
		    		error("cannot change constant value", n);
		    	}
		    	else if (rule.ac_par[0] == NEIGHBORS) {
		    		error("cannot change number of neighbors", n);
		    	}
		    	else if (rule.ac_par[0] == AGE) {
		    		error("cannot change cell age", n);
		    	}
		    	else if (rule.ac_par[0] == BIRTH) {
		    		error("cannot change cell birth", n);
		    	}
		    	else if (rule.ac_par[0] >= 2 * MAX_CHEMICALS) {
		    		error("cannot change map value", n);
		    	}

		    	// change quantity
		    	get_parameter(ss, rule.ac_par[1], rule.ac_val[1], n);
		    	ss >> word;
		    	if (word == "dev") {
		    		rule.ac_par[2] = CONSTANT;
		    		ss >> rule.ac_val[2];
		    	}
		    	else {
		    		rule.ac_par[2] = CONSTANT;
		    		rule.ac_val[2] = 0;
		    	}
		    }
		    else if (word == "map") {
		    	if (rule.predicate != ALWAYS) {
		    		error("map rule must have an 'always' predicate", n);
		    	}
		    	rule.action = MAP;
		    	float a, b;

		    	// source parameter and interval
		    	get_parameter(ss, rule.ac_par[0], rule.ac_val[0], n);
		    	ss >> a >> b;
		    	if (a == b) {
		    		error("map source limits cannot be equal", n);
		    	}
	    		rule.ac_par[1] = rule.ac_par[2] = CONSTANT;
	    		rule.ac_val[1] = a;
	    		rule.ac_val[2] = b;
	    		ss >> word;
	    		if (word != "to") {
	    			error("'to' expected in 'map' action", n);
	    		}

		    	// destination variable and range
	    		ss >> word;
	    		rule.ac_par[3] = (Parameter) (add_mapping(word, n) + 2 * MAX_CHEMICALS);
		    	ss >> a >> b;
	    		rule.ac_par[4] = rule.ac_par[5] = CONSTANT;
	    		rule.ac_val[4] = a;
	    		rule.ac_val[5] = b;
		    }
		    else if (word == "polarize") {
		    	rule.action = POLARIZE;

		    	// source parameter
		    	get_parameter(ss, rule.ac_par[0], rule.ac_val[0], n);
		    	if (rule.ac_par[0] < 0 || rule.ac_par[0] >= MAX_CHEMICALS) {
		    		error("polarity can only be based on a chemical concentration", n);
		    	}
		    }
		    else if (word == "divide") {
		    	rule.action = DIVIDE;
		    	ss >> word;
			    if (word == "direction") {
			    	get_parameter(ss, rule.ac_par[0], rule.ac_val[0], n);
			    	ss >> word;
			    	if (word == "dev") {
			    		rule.ac_par[1] = CONSTANT;
			    		ss >> rule.ac_val[1];
			    	}
			    	else {
			    		rule.ac_par[1] = CONSTANT;
			    		rule.ac_val[1] = 0;
			    	}
			    }
			    else {
			    	// no direction specified; inherit the same direction as parent
		    		rule.ac_par[0] = rule.ac_par[1] = CONSTANT;
		    		rule.ac_val[0] = rule.ac_val[1] = 0;
			    }
		    }
		    else if (word == "move") {
		    	rule.action = MOVE;
		    	get_parameter(ss, rule.ac_par[0], rule.ac_val[0], n);
		    	ss >> word;
		    	if (word == "dev") {
		    		rule.ac_par[1] = CONSTANT;
		    		ss >> rule.ac_val[1];
		    	}
		    	else {
		    		rule.ac_par[1] = CONSTANT;
		    		rule.ac_val[1] = 0;
		    	}
		    }
		    else if (word == "and") {
		    	rule.action = AND;
		    }
    		else {
    			error("unknown action " + word, n);
    		}
		    simulation_add_rule(rule);
	    }
	    else if (word == "colormap") {
	    	// skip colormap commands
	    	continue;
	    }
	    else if (word == "snap") {
	    	ss >> word;
	    	if (word == "at") {
	    		int snap = 0;
	    		while (ss >> word) {
	    			if (word == "then") {
	    				break;
	    			}
	    			std::stringstream(word) >> snap;
	    			simulation.snap_at.push_back(snap);
	    		}
	    	}
	    	else if (word == "from") {
	    		int start, repeat, step;
	    		ss >> start;
	    		ss >> word;
	    		if (word != "repeat") {
	    			error("expected 'repeat' keyword", n);
	    		}
	    		ss >> repeat;
	    		ss >> word;
	    		if (word != "step") {
	    			error("expected 'step' keyword", n);
	    		}
	    		ss >> step;
	    		for (int i = 0; i < repeat; i++) {
	    			simulation.snap_at.push_back(start);
	    			start += step;
	    		}
		    	ss >> word;
	    	}
	    	else {
	    		error("expected either 'at' or 'from' commands", n);
	    	}
		    if (word == "then") {
		    	ss >> word;
		    	if (word != "exit") {
		    		error("expected 'exit' command", n);
		    	}
		    	simulation.exit_at_end = true;
		    	continue;
		    }
	    	continue;
	    }
	    else if (word == "stop") {
	    	ss >> word;
	    	if (word != "at") {
	    		error("expected 'at' command", n);
	    	}
	    	ss >> simulation.stop_at;
	    	continue;
	    }
	    else if (word == "texture") {
	    	ss >> word;
	    	if (word != "size") {
	    		error("expected 'size' command", n);
	    	}
	    	ss >> simulation.texture_width;
	    	ss >> simulation.texture_height;
	    	continue;
	    }
	    else if (word == "zoom") {
	    	ss >> word;
	    	if (word != "level") {
	    		error("expected 'level' command", n);
	    	}
	    	ss >> simulation.zoom_level;
	    	continue;
	    }
	    else {
	    	error("unknown command " + word, n);
	    }
	}
	// no need to close file; let the destructor do it (so it can handle any exceptions raised)
}

void parser_load_colormap()
{
	std::ifstream file(file_name);
	if (! file.is_open()) {
		error("cannot open pattern file '" + std::string(file_name) + "'");
	}

	int n = 0;
	std::string line, word;
	while (std::getline(file, line)) {
	    n++;

		line = trim(line);
	    if (line.empty() || (line[0] == '/' && line[1] == '/')) {
	    	// skip comments and blank lines
	    	continue;
	    }

	    std::stringstream ss(line);
	    ss >> word;
	    if (word == "colormap") {
	    	/*---------------- colormap commands ----------------*/
	    	ss >> word;
		    if (word == "select") {
		    	ss >> word;
		    	if (word == "heat") {
		    		colormap_select(HEAT);
		    	}
		    	else if (word == "striped") {
		    		colormap_select(STRIPED);
		    	}
		    	else if (word == "gradient") {
		    		colormap_select(GRADIENT);
		    	}
		    	else {
		    		error("unknown colormap type", n);
		    	}
		    }
		    else if (word == "slot") {
		    	int i;
		    	ss >> i;
		    	ss >> word;
		    	if (word != "use") {
		    		error("expected 'use' command", n);
		    	}
		    	ss >> word;
		    	if (word == "color") {
		    		ss >> word;
			    	if (word[0] != '"'  || word[word.length() - 1] != '"') {
			    		error("color name must be delimited by double quotes", n);
			    	}
		    		word = word.substr(1, word.length() - 2);
		    		colormap_use_color(i, word.c_str());
		    	}
		    	else if (word == "rgb") {
		    		int r, g, b;
		    		ss >> r;
		    		ss >> g;
		    		ss >> b;
		    		colormap_use_rgb(i, r, g, b);
		    	}
		    	else {
		    		error("unknown color specification", n);
		    	}
		    }
		    else {
		    	error("unknown colormap command", n);
		    }
	    }
	}
	// no need to close file; let the destructor do it (so it can handle any exceptions raised)
}
