/*-------------------------------- INCLUDES --------------------------------*/

#include "nns_base.hpp"

/*-------------------------------- MACRO DEFINITIONS --------------------------------*/

#define X_COMPARE(A,B) (A.x > B.x)
#define Y_COMPARE(A,B) (A.y > B.y)

#define X_COMPARE_BOTH(A,B) ((A.x > B.x) || (A.x == B.x && A.y > B.y))
#define Y_COMPARE_BOTH(A,B) ((A.y > B.y) || (A.y == B.y && A.x > B.x))

/*-------------------------------- CONSTRUCTOR AND DESTRUCTOR --------------------------------*/

NNS_SpatialSorting::NNS_SpatialSorting(int m)
{
	counter = 0;
	curr_position = -1;
	positions = new Position[MAX_CELLS];

	dim_x = dim_y = 0;
	switch (m) {
	case 8:
	case 24:
	case 48:
	case 80:
	case 120:
		n_size = int(sqrtf((float) m + 1)) / 2;
		//std::cout << "nns: neighborhood m=" << m << ", n_size=" << n_size << '\n';
		break;
	default:
		std::cerr << "nns: invalid neighborhood m=" << m << ", m set to 48\n";
		n_size = 3;
	}
}

NNS_SpatialSorting::~NNS_SpatialSorting()
{
	delete[] positions; positions = NULL;
}

/*-------------------------------- PUBLIC METHOD IMPLEMENTATIONS --------------------------------*/

void NNS_SpatialSorting::add_position(float x, float y, CellId id)
{
	if (counter < MAX_CELLS) {
		positions[counter].set(x, y, id);
		counter++;

		int dim = int(ceilf(sqrtf((float) counter)));
		if (dim != dim_x) {
			// NOTE: a full sort should be run by the simulation, not here
			dim_x = dim_y = dim;
			//std::cout << "nns: matrix is now " << dim_x << " x " << dim_y << '\n';

			row_is_sorted.resize(dim_y);
			col_is_sorted.resize(dim_x);
		}
	}
	else {
		std::cerr << "error: no more positions available in add_position (max " << MAX_CELLS << ")\n";
	}
}

void NNS_SpatialSorting::update_position(CellId id, float x, float y)
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

void NNS_SpatialSorting::update_all_positions(Cell* cells)
{
	for (int index = 0; index < counter; index++) {
		CellId id = positions[index].cell_id;
		positions[index].x = cells[id].x;
		positions[index].y = cells[id].y;
	}
}

CellId NNS_SpatialSorting::locate_nearest(float x, float y)
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

void NNS_SpatialSorting::setup()
{
	spatial_odd_even_sort();

	//spatial_insertion_sort();
	//spatial_insertion_sort_skip();

	//spatial_quick_insertion_sort();
	//spatial_quick_insertion_sort_skip();

	//spatial_shell_insertion_sort();
	//spatial_shell_insertion_sort_skip();
}

void NNS_SpatialSorting::set_start_position()
{
	curr_position = -1;
}

bool NNS_SpatialSorting::has_next_position()
{
	curr_position++;
	return curr_position < counter;
}

CellId NNS_SpatialSorting::get_current_cell_id()
{
	return positions[curr_position].cell_id;
}

CellId *NNS_SpatialSorting::query_current_range(float r)
{
	return query_position_range(curr_position, r);
}

CellId *NNS_SpatialSorting::query_range(CellId id, float r)
{
	for (int index = 0; index < counter; index++) {
		if (positions[index].cell_id == id) {
			return query_position_range(index, r);
		}
	}
	return NULL; // 'id' does not exist
}

/*-------------------------------- PRIVATE METHOD IMPLEMENTATIONS --------------------------------*/

CellId *NNS_SpatialSorting::query_position_range(int index, float r)
{
	r = r * r;
	const Position& current = positions[index];
	get_hard_neighborhood(index);
	int *c = candidates;
	CellId *n = neighbors;

	while ((*c) != -1) {
		const Position& neighbor = positions[(*c)];
		float sqr_dist = (current.x - neighbor.x) * (current.x - neighbor.x) + (current.y - neighbor.y) * (current.y - neighbor.y);
		if (sqr_dist <= r) {
			(*n++) = neighbor.cell_id;
		}
		c++;
	}
	(*n) = -1; // mark list end

	return neighbors;
}

void NNS_SpatialSorting::get_hard_neighborhood(int index)
{
	int *c = candidates;
	int x = index % dim_x;
	int y = index / dim_x;
	int min_x = x - n_size, max_x = x + n_size;
	int min_y = y - n_size, max_y = y + n_size;
	if (min_x < 0) {
		min_x = 0;
	}
	else if (max_x >= dim_x) {
		max_x = dim_x - 1;
	}
	if (min_y < 0) {
		min_y = 0;
	}
	else if (max_y >= dim_y) {
		max_y = dim_y - 1;
	}
	for (int j = min_y; j <= max_y; j++) {
		for (int i = min_x; i <= max_x; i++) {
			int pos = i + j * dim_x;
			if (pos != index && pos < counter) {
				(*c++) = pos;
			}
		}
	}
	(*c) = -1; // mark list end
}

/*-------------------------------- SPATIAL ODD EVEN SORT --------------------------------*/

void NNS_SpatialSorting::spatial_odd_even_sort()
{
	bool is_sorted;
	do {
		is_sorted = true;

    	// (odd,even) pairwise comparison on x
    	for (int row = 0; row < dim_y; row++) {
    		for (int col = 1; col < dim_x - 1; col += 2) {
    			int curr = col + row * dim_x;
    			int neig = curr + 1;
    			Position& current  = positions[curr];
    			Position& neighbor = positions[neig];
    			if (X_COMPARE(current, neighbor)) {
    				Position temp = current;
    				positions[curr] = neighbor;
    				positions[neig] = temp;
    				is_sorted = false;
    			}
    		}
    	}

    	// (even,odd) pairwise comparison on x
    	for (int row = 0; row < dim_y; row++) {
    		for (int col = 0; col < dim_x - 1; col += 2) {
    			int curr = col + row * dim_x;
    			int neig = curr + 1;
    			Position& current  = positions[curr];
    			Position& neighbor = positions[neig];
    			if (X_COMPARE(current, neighbor)) {
    				Position temp = current;
    				positions[curr] = neighbor;
    				positions[neig] = temp;
    				is_sorted = false;
    			}
    		}
    	}

    	// (odd,even) pairwise comparison on y
    	for (int row = 1; row < dim_y - 1; row += 2) {
    		for (int col = 0; col < dim_x; col++) {
    			int curr = col + row * dim_x;
    			int neig = curr + dim_x;
    			Position& current  = positions[curr];
    			Position& neighbor = positions[neig];
    			if (Y_COMPARE(current, neighbor)) {
    				Position temp = current;
    				positions[curr] = neighbor;
    				positions[neig] = temp;
    				is_sorted = false;
    			}
    		}
    	}

    	// (even,odd) pairwise comparison on y
    	for (int row = 0; row < dim_y - 1; row += 2) {
    		for (int col = 0; col < dim_x; col++) {
    			int curr = col + row * dim_x;
    			int neig = curr + dim_x;
    			Position& current  = positions[curr];
    			Position& neighbor = positions[neig];
    			if (Y_COMPARE(current, neighbor)) {
    				Position temp = current;
    				positions[curr] = neighbor;
    				positions[neig] = temp;
    				is_sorted = false;
    			}
    		}
    	}
    } while (! is_sorted);
}

/*-------------------------------- SPATIAL INSERTION SORT --------------------------------*/

void NNS_SpatialSorting::spatial_insertion_sort()
{
	int writes;
	do {
		writes = standard_insertion_sort_on_each_row();
		if (writes == 0) {
			break;
		}
		writes = standard_insertion_sort_on_each_col();
	} while (writes != 0);
}

int NNS_SpatialSorting::standard_insertion_sort_on_each_row()
{
	int writes = 0;
	for (int row = 0; row < dim_y; row++) {
		for (int i = 1; i < dim_x; i++) {
			Position temp = positions[i + row * dim_x];
			int j;
			for (j = i; j >= 1; j--) {
				Position curr = positions[(j - 1) + row * dim_x];
				if (X_COMPARE(curr, temp)) {
					positions[j + row * dim_x] = curr;
					writes++;
				}
				else {
					break;
				}
			}
			if (j != i) {
				positions[j + row * dim_x] = temp;
				writes++;
			}
		}
	}
    return writes;
}

int NNS_SpatialSorting::standard_insertion_sort_on_each_col()
{
	int writes = 0;
	for (int col = 0; col < dim_x; col++) {
		for (int i = 1; i < dim_y; i++) {
			Position temp = positions[col + i * dim_x];
			int j;
			for (j = i; j >= 1; j--) {
				Position curr = positions[col + (j - 1) * dim_x];
				if (Y_COMPARE(curr, temp)) {
					positions[col + j * dim_x] = curr;
					writes++;
				}
				else {
					break;
				}
			}
			if (j != i) {
				positions[col + j * dim_x] = temp;
				writes++;
			}
		}
	}
    return writes;
}

void NNS_SpatialSorting::spatial_insertion_sort_skip()
{
	standard_insertion_sort_on_each_row();
	int writes = standard_insertion_sort_on_each_col();

    std::fill(row_is_sorted.begin(), row_is_sorted.end(), false);
    std::fill(col_is_sorted.begin(), col_is_sorted.end(), false);

	while (writes != 0) {
		writes = standard_insertion_sort_on_each_row_skip();
		if (writes == 0) {
			break;
		}
		writes = standard_insertion_sort_on_each_col_skip();
	}
}

int NNS_SpatialSorting::standard_insertion_sort_on_each_row_skip()
{
	int writes = 0;
	for (int row = 0; row < dim_y; row++) {
		if (row_is_sorted[row]) {
			continue;
		}
		for (int i = 1; i < dim_x; i++) {
			Position temp = positions[i + row * dim_x];
			int j;
			for (j = i; j >= 1; j--) {
				Position curr = positions[(j - 1) + row * dim_x];
				if (X_COMPARE(curr, temp)) {
					positions[j + row * dim_x] = curr;
					writes++;
					col_is_sorted[j] = false;
				}
				else {
					break;
				}
			}
			if (j != i) {
				positions[j + row * dim_x] = temp;
				writes++;
				col_is_sorted[j] = false;
			}
		}
		row_is_sorted[row] = true;
	}
    return writes;
}

int NNS_SpatialSorting::standard_insertion_sort_on_each_col_skip()
{
	int writes = 0;
	for (int col = 0; col < dim_x; col++) {
		if (col_is_sorted[col]) {
			continue;
		}
		for (int i = 1; i < dim_y; i++) {
			Position temp = positions[col + i * dim_x];
			int j;
			for (j = i; j >= 1; j--) {
				Position curr = positions[col + (j - 1) * dim_x];
				if (Y_COMPARE(curr, temp)) {
					positions[col + j * dim_x] = curr;
					writes++;
					row_is_sorted[j] = false;
				}
				else {
					break;
				}
			}
			if (j != i) {
				positions[col + j * dim_x] = temp;
				writes++;
				row_is_sorted[j] = false;
			}
		}
		col_is_sorted[col] = true;
	}
    return writes;
}

/*-------------------------------- SPATIAL QUICK + INSERTION SORT --------------------------------*/

void NNS_SpatialSorting::spatial_quick_insertion_sort()
{
	standard_quicksort_on_each_row();
	standard_quicksort_on_each_col();
	spatial_insertion_sort();
}

void NNS_SpatialSorting::spatial_quick_insertion_sort_skip()
{
	standard_quicksort_on_each_row();
	standard_quicksort_on_each_col();
	spatial_insertion_sort_skip();
}

int NNS_SpatialSorting::standard_partition_on_row(int row, int left, int right, int pivot_index)
{
	Position pivot = positions[pivot_index + row * dim_x];
	positions[pivot_index + row * dim_x] = positions[right + row * dim_x];

	int store_index = left;
    for (int i = left; i < right; i++) {
    	Position curr = positions[i + row * dim_x];
    	if (X_COMPARE(pivot, curr)) {
    		Position store = positions[store_index + row * dim_x];
    		positions[store_index + row * dim_x] = curr;
    		positions[i + row * dim_x] = store;
    		store_index++;
    	}
    }
    positions[right + row * dim_x] = positions[store_index + row * dim_x];
    positions[store_index + row * dim_x] = pivot;
    return store_index;
}

void NNS_SpatialSorting::standard_quicksort_on_row(int row, int left, int right)
{
    if (left < right) {
    	int pivot_index = (left + right) / 2;
        int new_pivot_index = standard_partition_on_row(row, left, right, pivot_index);
        standard_quicksort_on_row(row, left, new_pivot_index - 1);
        standard_quicksort_on_row(row, new_pivot_index + 1, right);
    }
}

void NNS_SpatialSorting::standard_quicksort_on_each_row()
{
	for (int row = 0; row < dim_y; row++) {
		standard_quicksort_on_row(row, 0, dim_x - 1);
	}
}

int NNS_SpatialSorting::standard_partition_on_col(int col, int left, int right, int pivot_index)
{
	Position pivot = positions[col + pivot_index * dim_x];
	positions[col + pivot_index * dim_x] = positions[col + right * dim_x];

	int store_index = left;
    for (int i = left; i < right; i++) {
    	Position curr = positions[col + i * dim_x];
    	if (Y_COMPARE(pivot, curr)) {
    		Position store = positions[col + store_index * dim_x];
    		positions[col + store_index * dim_x] = curr;
    		positions[col + i * dim_x] = store;
    		store_index++;
    	}
    }
    positions[col + right * dim_x] = positions[col + store_index * dim_x];
    positions[col + store_index * dim_x] = pivot;
    return store_index;
}

void NNS_SpatialSorting::standard_quicksort_on_col(int col, int left, int right)
{
    if (left < right) {
    	int pivot_index = (left + right) / 2;
        int new_pivot_index = standard_partition_on_col(col, left, right, pivot_index);
        standard_quicksort_on_col(col, left, new_pivot_index - 1);
        standard_quicksort_on_col(col, new_pivot_index + 1, right);
    }
}

void NNS_SpatialSorting::standard_quicksort_on_each_col()
{
	for (int col = 0; col < dim_x; col++) {
		standard_quicksort_on_col(col, 0, dim_y - 1);
	}
}

/*-------------------------------- SPATIAL SHELL + INSERTION SORT --------------------------------*/

void NNS_SpatialSorting::spatial_shell_insertion_sort()
{
	spatial_shell_sort();
	spatial_insertion_sort();
}

void NNS_SpatialSorting::spatial_shell_insertion_sort_skip()
{
	spatial_shell_sort();
	spatial_insertion_sort_skip();
}

void NNS_SpatialSorting::spatial_shell_sort()
{
	int gaps[] = {1750, 701, 301, 132, 57, 23, 10, 4, 1};

	for (int g = 0; g < (int) (sizeof(gaps) / sizeof(int)); g++) {
		int gap = gaps[g];

   		// sort rows using current gap
	   	if (gap < dim_x) {
	   		// for each row
	   		for (int row = 0; row < dim_y; row++) {
	   			// do a gapped insertion sort on this row
	   			for (int i = gap; i < dim_x; i++) {
	   				// add a[i] to the elements that have been gap sorted
	   				// save a[i] in temp and make a hole at position i
	   				Position temp = positions[i + row * dim_x];
	   				// shift earlier gap-sorted elements up until the correct location for a[i] is found
	   				int j;
	   				for (j = i; j >= gap; j -= gap) {
	   					Position curr = positions[(j - gap) + row * dim_x];
	   					if (X_COMPARE_BOTH(curr, temp)) {
	   						positions[j + row * dim_x] = curr;
	   					}
	   					else {
	   						break;
	   					}
	   				}
	   				// put temp (the original a[i]) in its correct location, if changed
	   				if (j != i) {
	   					positions[j + row * dim_x] = temp;
	   				}
	   			}
	   		}
	   	}

   		// sort columns using current gap
	   	if (gap < dim_y) {
	   		// for each column
	   		for (int col = 0; col < dim_x; col++) {
	   			// do a gapped insertion sort on this column
	   			for (int i = gap; i < dim_y; i++) {
	   				// add a[i] to the elements that have been gap sorted
	   				// save a[i] in temp and make a hole at position i
	   				Position temp = positions[col + i * dim_x];
	   				// shift earlier gap-sorted elements up until the correct location for a[i] is found
	   				int j;
	   				for (j = i; j >= gap; j -= gap) {
	   					Position curr = positions[col + (j - gap) * dim_x];
	   					if (Y_COMPARE_BOTH(curr, temp)) {
	   						positions[col + j * dim_x] = curr;
	   					}
	   					else {
	   						break;
	   					}
	   				}
	   				// put temp (the original a[i]) in its correct location, if changed
	   				if (j != i) {
	   					positions[col + j * dim_x] = temp;
	   				}
	   			}
	   		}
	   	}
	}
}
