/*-------------------------------- INCLUDES --------------------------------*/

#include <cstdlib>

#include "nns_base.hpp"

/*-------------------------------- CONSTRUCTOR AND DESTRUCTOR --------------------------------*/

NNS_SquareGrid::NNS_SquareGrid(int dim_x, int dim_y, bool wrap)
{
	counter = 0;
	curr_position = -1;
	positions = new Position[MAX_CELLS];

	if (dim_x == 1 || dim_y == 1) {
		std::cerr << "nns: square grid does not accept unit dimensions: " << dim_x << " x " << dim_y << '\n';
		PAUSE_AND_EXIT;
	}
	this->dim_x = dim_x;
	this->dim_y = dim_y;
	this->wrap = wrap;
}

NNS_SquareGrid::~NNS_SquareGrid()
{
	delete[] positions; positions = NULL;
}

/*-------------------------------- PUBLIC METHOD IMPLEMENTATIONS --------------------------------*/

void NNS_SquareGrid::add_position(float x, float y, CellId id)
{
	if (counter < MAX_CELLS) {
		positions[counter].set(x, y, id);
		counter++;
	}
	else {
		std::cerr << "error: no more positions available in add_position (max " << MAX_CELLS << ")\n";
	}
}

void NNS_SquareGrid::update_position(CellId id, float x, float y)
{
	// NOTE: should we use a more efficient algorithm here?
	for (int index = 0; index < counter; index++) {
		if (positions[index].cell_id == id) {
			positions[index].x = x;
			positions[index].y = y;
			return;
		}
	}
	std::cerr << "error: " << id << " not found in update_position\n";
}

void NNS_SquareGrid::update_all_positions(Cell* cells)
{
	for (int index = 0; index < counter; index++) {
		CellId id = positions[index].cell_id;
		positions[index].x = cells[id].x;
		positions[index].y = cells[id].y;
	}
}

CellId NNS_SquareGrid::locate_nearest(float x, float y)
{
	// NOTE: should we use a more efficient algorithm here?
	float min_dist = FLT_MAX;
	int nearest = -1;
	Position p(x, y, (CellId) -1);
	for (int index = 0; index < counter; index++) {
		const Position& o = positions[index];
		float sqr_dist = (p.x - o.x) * (p.x - o.x) + (p.y - o.y) * (p.y - o.y);
		if (sqr_dist < min_dist) {
			min_dist = sqr_dist;
			nearest = index;
		}
	}
	return positions[nearest].cell_id;
}

void NNS_SquareGrid::setup()
{
}

void NNS_SquareGrid::set_start_position()
{
	curr_position = -1;
}

bool NNS_SquareGrid::has_next_position()
{
	curr_position++;
	return curr_position < counter;
}

CellId NNS_SquareGrid::get_current_cell_id()
{
	return positions[curr_position].cell_id;
}

CellId *NNS_SquareGrid::query_current_range(float r)
{
	return query_position_range(curr_position, r);
}

CellId *NNS_SquareGrid::query_range(CellId id, float r)
{
	for (int index = 0; index < counter; index++) {
		if (positions[index].cell_id == id) {
			return query_position_range(index, r);
		}
	}
	return NULL; // 'id' does not exist
}

/*-------------------------------- PRIVATE METHOD IMPLEMENTATIONS --------------------------------*/

CellId *NNS_SquareGrid::query_position_range(int index, UNUSED float r)
{
	CellId *n = neighbors;

	int row = index / dim_x;
	int col = index % dim_x;

	if (wrap) {
		int rowm = (row == 0) ? dim_y - 1 : row - 1;
		int rowp = (row == dim_y - 1) ? 0 : row + 1;

		int colm = (col == 0) ? dim_x - 1 : col - 1;
		int colp = (col == dim_x - 1) ? 0 : col + 1;

		(*n++) = rowm * dim_x + colm;
		(*n++) = rowm * dim_x + col;
		(*n++) = rowm * dim_x + colp;
		(*n++) = row * dim_x + colm;
		(*n++) = row * dim_x + colp;
		(*n++) = rowp * dim_x + colm;
		(*n++) = rowp * dim_x + col;
		(*n++) = rowp * dim_x + colp;
		(*n) = -1; // mark list end

		return neighbors;
	}

	if (row == dim_y - 1) {
		// top row
		if (col == 0) {
			// left column
			(*n++) = index + 1;
			(*n++) = index - dim_x;
			(*n++) = index - dim_x + 1;
		}
		else if (col < dim_x - 1) {
			// one of the center columns
			(*n++) = index - 1;
			(*n++) = index + 1;
			(*n++) = index - dim_x - 1;
			(*n++) = index - dim_x;
			(*n++) = index - dim_x + 1;
		}
		else {
			// right column
			(*n++) = index - 1;
			(*n++) = index - dim_x - 1;
			(*n++) = index - dim_x;
		}
	}
	else if (row > 0) {
		// one of the middle rows
		if (col == 0) {
			// left column
			(*n++) = index - dim_x;
			(*n++) = index - dim_x + 1;
			(*n++) = index + 1;
			(*n++) = index + dim_x;
			(*n++) = index + dim_x + 1;
		}
		else if (col < dim_x - 1) {
			// one of the center columns -- most common case
			(*n++) = index - dim_x - 1;
			(*n++) = index - dim_x;
			(*n++) = index - dim_x + 1;
			(*n++) = index - 1;
			(*n++) = index + 1;
			(*n++) = index + dim_x - 1;
			(*n++) = index + dim_x;
			(*n++) = index + dim_x + 1;
		}
		else {
			// right column
			(*n++) = index - dim_x - 1;
			(*n++) = index - dim_x;
			(*n++) = index - 1;
			(*n++) = index + dim_x - 1;
			(*n++) = index + dim_x;
		}
	}
	else {
		// bottom row
		if (col == 0) {
			// left column
			(*n++) = index + dim_x;
			(*n++) = index + dim_x + 1;
			(*n++) = index + 1;
		}
		else if (col < dim_x - 1) {
			// one of the center columns
			(*n++) = index + dim_x - 1;
			(*n++) = index + dim_x;
			(*n++) = index + dim_x + 1;
			(*n++) = index - 1;
			(*n++) = index + 1;
		}
		else {
			// right column
			(*n++) = index + dim_x - 1;
			(*n++) = index + dim_x;
			(*n++) = index - 1;
		}
	}
	(*n) = -1; // mark list end

	return neighbors;
}
