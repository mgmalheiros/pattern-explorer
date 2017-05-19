/*-------------------------------- INCLUDES --------------------------------*/

#include "nanoflann.hpp"
#include "nns_base.hpp"

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, NNS_KD_Tree>, NNS_KD_Tree, 2 /* dimension */> KDTree;

/*-------------------------------- LOCAL VARIABLES --------------------------------*/

// do not sort radius search results
static nanoflann::SearchParams params(0, 0.0, false);

/*-------------------------------- CONSTRUCTOR AND DESTRUCTOR --------------------------------*/

NNS_KD_Tree::NNS_KD_Tree()
{
	counter = 0;
	curr_position = -1;
    positions = new Position[MAX_CELLS];

    kd_tree = new KDTree(2 /* dimension */, (*this), nanoflann::KDTreeSingleIndexAdaptorParams(100 /* max leaf */));
}

NNS_KD_Tree::~NNS_KD_Tree()
{
    delete[] positions; positions = NULL;
    delete (KDTree *) kd_tree; kd_tree = NULL;
}

/*-------------------------------- PUBLIC METHOD IMPLEMENTATIONS --------------------------------*/

void NNS_KD_Tree::add_position(float x, float y, CellId id)
{
	if (counter < MAX_CELLS) {
		positions[counter].set(x, y, id);
		counter++;
	}
	else {
		std::cerr << "error: no more positions available in add_position (max " << MAX_CELLS << ")\n";
	}
}

void NNS_KD_Tree::update_position(CellId id, float x, float y)
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

void NNS_KD_Tree::update_all_positions(Cell* cells)
{
	for (int index = 0; index < counter; index++) {
		CellId id = positions[index].cell_id;
		positions[index].x = cells[id].x;
		positions[index].y = cells[id].y;
	}
}

CellId NNS_KD_Tree::locate_nearest(float x, float y)
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

void NNS_KD_Tree::setup()
{
	((KDTree *) (kd_tree))->buildIndex();
}

void NNS_KD_Tree::set_start_position()
{
	curr_position = -1;
}

bool NNS_KD_Tree::has_next_position()
{
	curr_position++;
	return curr_position < counter;
}

CellId NNS_KD_Tree::get_current_cell_id()
{
	return positions[curr_position].cell_id;
}

CellId *NNS_KD_Tree::query_current_range(float r)
{
	return query_position_range(curr_position, r);
}

CellId *NNS_KD_Tree::query_range(CellId id, float r)
{
	for (int index = 0; index < counter; index++) {
		if (positions[index].cell_id == id) {
			return query_position_range(index, r);
		}
	}
	return NULL; // 'id' does not exist
}

/*-------------------------------- PRIVATE METHOD IMPLEMENTATIONS --------------------------------*/

CellId *NNS_KD_Tree::query_position_range(int index, float r)
{
	query_pt[0] = positions[index].x;
	query_pt[1] = positions[index].y;

	// Find all the neighbors to query_point[0:dim-1] within a maximum radius.
	// The output is given as a vector of pairs, of which the first element is a point index and the
	// second the corresponding distance. Previous contents of 'ret_matches' are cleared.
	const int n = ((KDTree*) kd_tree)->radiusSearch(&query_pt[0], r * r, ret_matches, params);

	int j = 0;
	for (int i = 0; i < n; i++) {
		// skip self
		if ((int) ret_matches[i].first != index) {
			// NOTE: this works now because index matches CellId; in the general case we would need
			// neighbors[i] = positions[ret_matches[i].first].cell_id;
			neighbors[j++] = ret_matches[i].first;
		}
	}
	neighbors[j] = -1;

	return neighbors;
}

#ifdef FUTURE

//size_t nanoflann::KDTreeSingleIndexAdaptor< Distance, DatasetAdaptor, DIM, IndexType >::usedMemory	(		)	const
//inline
//Computes the index memory usage Returns: memory used by the index

void query_nanoflann(int k)
{
	float query_pt[2];
	std::vector<size_t> ret_index(k + 1);
	std::vector<float> out_dist_sqr(k + 1);

	// ----------------------------------------------------------------
	// knnSearch():  Perform a search for the N closest points
	// ----------------------------------------------------------------
	for (int i = 0; i < position_counter; i++) {
		query_pt[0] = position_matrix[i].x;
		query_pt[1] = position_matrix[i].y;
		nano_tree->knnSearch(&query_pt[0], k + 1, &ret_index[0], &out_dist_sqr[0]);

		for (int j = 0; j < k; j++) {
			nearest_trial[i * k + j] = out_dist_sqr[j + 1]; // store squared distance of nearest point
		}

	}

}

#endif
