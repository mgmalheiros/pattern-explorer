#ifndef NNS_BASE_HPP
#define NNS_BASE_HPP

#include "types.hpp"

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

class Position {
public:
	float  x, y;
	CellId cell_id;

    Position()
	{
    	x = y = FLT_MAX;
    	cell_id = -1;
	}

    Position(float x, float y, CellId cell_id)
    {
    	this->x = x;
    	this->y = y;
    	this->cell_id = cell_id;
    }

    void set(float x, float y, CellId cell_id)
    {
    	this->x = x;
    	this->y = y;
    	this->cell_id = cell_id;
    }
};

/*-------------------------------- INTERFACES --------------------------------*/

class NNS {
public:
    virtual ~NNS() {}

    virtual void add_position(float x, float y, CellId id) = 0;
    virtual void update_position(CellId id, float x, float y) = 0;
    virtual void update_all_positions(Cell* cells) = 0;

    virtual CellId locate_nearest(float x, float y) = 0;

    virtual void setup() = 0;

    virtual void set_start_position() = 0;
    virtual bool has_next_position() = 0;
    virtual CellId get_current_cell_id() = 0;
    virtual CellId *query_current_range(float r) = 0;

    virtual CellId *query_range(CellId id, float r) = 0;
    // TODO: virtual CellId *query_nearest(CellId id, int k) = 0;

    // TODO: get used memory
    // TODO: get total memory
};

/*-------------------------------- IMPLEMENTATIONS --------------------------------*/

class NNS_KD_Tree : public NNS {
private:
	int counter, curr_position;
    Position *positions;

    void *kd_tree;
    CellId neighbors[500]; // TODO: remove hard-coded limit

	std::vector<std::pair<size_t,float> > ret_matches;
	float query_pt[2];

public:
    NNS_KD_Tree();
    ~NNS_KD_Tree();

    void add_position(float x, float y, CellId id);
    void update_position(CellId id, float x, float y);
    void update_all_positions(Cell* cells);

    CellId locate_nearest(float x, float y);

    void setup();

    void set_start_position();
    bool has_next_position();
    CellId get_current_cell_id();
    CellId *query_current_range(float r);

    CellId *query_range(CellId id, float r);
    //CellId *query_nearest(CellId id, int k);

    // -------- Methods for Nanoflann adaptor interface --------

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return counter; }

    // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class
    inline float kdtree_distance(const float *p1, const size_t idx_p2, UNUSED size_t size) const
    {
    	const float d0 = p1[0] - positions[idx_p2].x;
    	const float d1 = p1[1] - positions[idx_p2].y;
    	return d0 * d0 + d1 * d1;
    }

    // Returns the dim'th component of the idx'th point in the class
    inline float kdtree_get_pt(const size_t idx, int dim) const
    {
    	if (dim == 0) {
    		return positions[idx].x;
    	}
    	else {
    		return positions[idx].y;
    	}
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(UNUSED BBOX &bb) const { return false; }

private:
    CellId *query_position_range(int index, float r);
};

class NNS_SpatialSorting : public NNS {
private:
	int counter, curr_position;
    Position *positions;

    int dim_x, dim_y;
	int n_size;
    int candidates[121];
    CellId neighbors[121];

    std::vector<bool> row_is_sorted;
    std::vector<bool> col_is_sorted;

public:
    NNS_SpatialSorting(int m);
    ~NNS_SpatialSorting();

    void add_position(float x, float y, CellId id);
    void update_position(CellId id, float x, float y);
    void update_all_positions(Cell* cells);

    CellId locate_nearest(float x, float y);

    void setup();

    void set_start_position();
    bool has_next_position();
    CellId get_current_cell_id();
    CellId *query_current_range(float r);

    CellId *query_range(CellId id, float r);
    //CellId *query_nearest(CellId id, int k);

private:
    CellId *query_position_range(int index, float r);
    void get_hard_neighborhood(int index);

    void spatial_odd_even_sort();

    void spatial_insertion_sort();
    int standard_insertion_sort_on_each_row();
    int standard_insertion_sort_on_each_col();

    void spatial_insertion_sort_skip();
    int standard_insertion_sort_on_each_row_skip();
    int standard_insertion_sort_on_each_col_skip();

    void spatial_quick_insertion_sort();
    void spatial_quick_insertion_sort_skip();
    int standard_partition_on_row(int row, int left, int right, int pivot_index);
    void standard_quicksort_on_row(int row, int left, int right);
    void standard_quicksort_on_each_row();
    int standard_partition_on_col(int col, int left, int right, int pivot_index);
    void standard_quicksort_on_col(int col, int left, int right);
    void standard_quicksort_on_each_col();

    void spatial_shell_insertion_sort();
    void spatial_shell_insertion_sort_skip();
    void spatial_shell_sort();
};

class NNS_SquareGrid : public NNS {
private:
	int counter, curr_position;
    Position *positions;

    int dim_x, dim_y;
    CellId neighbors[9];
    bool wrap;

public:
    NNS_SquareGrid(int dim_x, int dim_y, bool wrap);
    ~NNS_SquareGrid();

    void add_position(float x, float y, CellId id);
    void update_position(CellId id, float x, float y);
    void update_all_positions(Cell* cells);

    CellId locate_nearest(float x, float y);

    void setup();

    void set_start_position();
    bool has_next_position();
    CellId get_current_cell_id();
    CellId *query_current_range(float r);

    CellId *query_range(CellId id, float r);
    //CellId *query_nearest(CellId id, int k);

private:
    CellId *query_position_range(int index, float r);
    void get_hard_neighborhood(int index);
};

#endif // NNS_BASE_HPP
