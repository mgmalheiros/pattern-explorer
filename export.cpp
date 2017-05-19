/*-------------------------------- INCLUDES --------------------------------*/

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Interpolation_traits_2.h>
#include <CGAL/natural_neighbor_coordinates_2.h>
#include <CGAL/interpolation_functions.h>

#include <png.h>

#include "colormap.hpp"
#include "simulation.hpp"
#include "types.hpp"

#include "export.hpp"

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

typedef CGAL::Exact_predicates_inexact_constructions_kernel           K;
typedef CGAL::Delaunay_triangulation_2<K>                             Delaunay_triangulation;
typedef CGAL::Interpolation_traits_2<K>                               Traits;
typedef K::FT                                                         Coord_type;
typedef K::Point_2                                                    Point;
typedef CGAL::Data_access<std::map<Point, Coord_type, K::Less_xy_2> > Value_access;

/*-------------------------------- LOCAL VARIABLES --------------------------------*/

static int tex_counter = 0;
//static int vec_counter = 0;

static png_byte **rows = NULL;

static unsigned char *image = NULL;

//static std::ofstream svg;

/*-------------------------------- INTERPOLATION FUNCTIONS --------------------------------*/

// adapted from http://stackoverflow.com/questions/3191978/how-to-use-glut-opengl-to-render-to-a-file

void export_png(const char *filename, int width, int height, unsigned char *pixels)
{
    rows = (png_byte **) realloc(rows, height * sizeof(png_byte *));
    for (int i = 0; i < height; i++) {
        rows[height - i - 1] = &pixels[i * width * 3];
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
    	fprintf(stderr, "png writing failed!\n");
    	abort();
    }
    png_infop info = png_create_info_struct(png);
    if (!info) {
    	fprintf(stderr, "png writing failed!\n");
    	abort();
    }
    if (setjmp(png_jmpbuf(png))) {
    	fprintf(stderr, "png writing failed!\n");
    	abort();
    }

    FILE *f = fopen(filename, "wb");
    png_init_io(png, f);
    png_set_IHDR(
        png,
        info,
        width,
        height,
        8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);
    png_write_image(png, rows);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(f);
}

void export_texture(int chemical, int width, int height)
{
	Delaunay_triangulation T;
	std::map<Point, Coord_type, K::Less_xy_2> function_values;

    for (int i = 0; i < simulation.n_cells; i++) {
		Cell& curr_cell = simulation.curr_cells[i];
		float x = curr_cell.x;
		float y = curr_cell.y;
		float v = curr_cell.conc[chemical];

		// add point to Delaunay triangulation and store associated value
		K::Point_2 p(x, y);
		T.insert(p);
		function_values.insert(std::make_pair(p, v));
	}

    //std::cout << "v min=" << statistics.chem_min[0] << " v max=" << statistics.chem_max[0] << '\n';
    //std::cout << "x min=" << statistics.cell_xmin   << " x max=" << statistics.cell_xmax << '\n';
    //std::cout << "y min=" << statistics.cell_ymin   << " y max=" << statistics.cell_ymax << '\n';

    float xrange = statistics.cell_xmax - statistics.cell_xmin;
    float yrange = statistics.cell_ymax - statistics.cell_ymin;
    float xmin = statistics.cell_xmin; // + (statistics.cell_xmax - statistics.cell_xmin - range) / 2;
    float ymin = statistics.cell_ymin; // + (statistics.cell_ymax - statistics.cell_ymin - range) / 2;
    colormap_set_limits(statistics.chem_min[chemical], statistics.chem_max[chemical]);

    image = (unsigned char *) realloc(image, width * height * 3 * sizeof(unsigned char));

	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {
			int index = (r * width + c) * 3;
			float x = xmin + xrange * c / (width - 1);
			float y = ymin + yrange * r / (height - 1);

			// query point
			K::Point_2 q(x, y);

			// get natural coordinates for query point
			std::vector<std::pair<Point, Coord_type> > coords;
			Coord_type norm = CGAL::natural_neighbor_coordinates_2(T, q, std::back_inserter(coords)).second;

			if (coords.size()) {
				// query point is inside the convex hull: calculate interpolated value
				Coord_type val = CGAL::linear_interpolation(coords.begin(), coords.end(), norm, Value_access(function_values));

				// lookup colormap and convert to RGB
				float *color = colormap_lookup(val);
				image[index + 0] = (int) (color[0] * 255);
				image[index + 1] = (int) (color[1] * 255);
				image[index + 2] = (int) (color[2] * 255);
			}
			else {
				// query point is outside: use background color
				image[index + 0] = 0;
				image[index + 1] = 0;
				image[index + 2] = 0;
			}
		}
	}

	char filename[256];
	sprintf(filename, "tex-%02d.png", tex_counter);
	export_png(filename, width, height, image);
	tex_counter++;
}

/*void export_texture_wrap(int chemical, int width, int height, int nns_dim_x, int nns_dim_y)
{
	Delaunay_triangulation T;
	std::map<Point, Coord_type, K::Less_xy_2> function_values;

	for (int r = -1; r <= nns_dim_y; r++) {
		int j = (r + nns_dim_y) % nns_dim_y;
		for (int c = -1; c <= nns_dim_x; c++) {
			int i = (c + nns_dim_x) % nns_dim_x;
			Cell& curr_cell = simulation.curr_cells[i + j * nns_dim_x];
			float x = curr_cell.x;
			float y = curr_cell.y;
			float v = curr_cell.conc[chemical];
			if (c == -1) { x -= 2 * nns_dim_x; } else if (c == nns_dim_x) { x += 2 * nns_dim_x; }
			if (r == -1) { y -= 2 * nns_dim_x; } else if (r == nns_dim_y) { y += 2 * nns_dim_x; }

			// add point to Delaunay triangulation and store associated value
			K::Point_2 p(x, y);
			T.insert(p);
			function_values.insert(std::make_pair(p, v));
		}
	}

    //std::cout << "v min=" << statistics.chem_min[0] << " v max=" << statistics.chem_max[0] << '\n';
    //std::cout << "x min=" << statistics.cell_xmin   << " x max=" << statistics.cell_xmax << '\n';
    //std::cout << "y min=" << statistics.cell_ymin   << " y max=" << statistics.cell_ymax << '\n';

    float xrange = statistics.cell_xmax - statistics.cell_xmin + 1;
    float yrange = statistics.cell_ymax - statistics.cell_ymin + 1;
    float xmin = statistics.cell_xmin - 1; // + (statistics.cell_xmax - statistics.cell_xmin - range) / 2;
    float ymin = statistics.cell_ymin - 1; // + (statistics.cell_ymax - statistics.cell_ymin - range) / 2;
    colormap_set_limits(statistics.chem_min[chemical], statistics.chem_max[chemical]);

    image = (unsigned char *) realloc(image, width * height * 3 * sizeof(unsigned char));

	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {
			int index = (r * width + c) * 3;
			float x = xmin + xrange * c / (width - 1);
			float y = ymin + yrange * r / (height - 1);

			// query point
			K::Point_2 q(x, y);

			// get natural coordinates for query point
			std::vector<std::pair<Point, Coord_type> > coords;
			Coord_type norm = CGAL::natural_neighbor_coordinates_2(T, q, std::back_inserter(coords)).second;

			if (coords.size()) {
				// query point is inside the convex hull: calculate interpolated value
				Coord_type val = CGAL::linear_interpolation(coords.begin(), coords.end(), norm, Value_access(function_values));

				// lookup colormap and convert to RGB
				float *color = colormap_lookup(val);
				image[index + 0] = (int) (color[0] * 255);
				image[index + 1] = (int) (color[1] * 255);
				image[index + 2] = (int) (color[2] * 255);
			}
			else {
				// query point is outside: use background color
				image[index + 0] = 0;
				image[index + 1] = 0;
				image[index + 2] = 0;
			}
		}
	}

	char filename[256];
	sprintf(filename, "tex-%02d.png", tex_counter);
	export_png(filename, width, height, image);
	tex_counter++;
}*/

#ifdef OFF

void vector_open_file()
{
	svg.open("vector.svg");
	if (! svg.is_open()) {
		std::cerr << "cannot open vector file\n";
		exit(1);
	}

	// output header
	svg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
	svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ";
	svg << "width=\""  << (simulation.domain_xmax - simulation.domain_xmin) << "\" ";
	svg << "height=\"" << (simulation.domain_ymax - simulation.domain_ymin) << "\">\n";
	svg << "  <g transform=\"scale(1,-1) translate(" << (- simulation.domain_xmin) << "," << simulation.domain_ymin << ")\">\n";
}

void vector_draw_cell(float x, float y, float r)
{
	float color[4];
	glGetFloatv(GL_CURRENT_COLOR, color);

	char str[10];
	sprintf(str, "#%02x%02x%02x", (int) (color[0] * 255), (int) (color[1] * 255), (int) (color[2] * 255));

	svg << "    <circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r << "\" ";
	svg << "stroke=\"black\" stroke-width=\"0.5\" fill=\"" << str << "\" />\n";
	//svg << "stroke=\"" << str << "\" stroke-width=\"1\" fill=\"none\" />\n";
}

void vector_close_file()
{
	svg << "  </g>\n";
	svg << "</svg>\n";
	svg.close();
	std::cout << "saved vector file!\n";
}

#endif
