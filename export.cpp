/*-------------------------------- COMPATIBILITY --------------------------------*/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

/*-------------------------------- INCLUDES --------------------------------*/

#include <cstdio>
#include <cstdlib>

#ifdef ENABLE_PNG
#include <png.h>
#endif // ENABLE_PNG

#ifdef ENABLE_CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Interpolation_traits_2.h>
#include <CGAL/natural_neighbor_coordinates_2.h>
#include <CGAL/interpolation_functions.h>
#endif // ENABLE_CGAL

#include "colormap.hpp"
#include "simulation.hpp"
#include "types.hpp"

#include "export.hpp"

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

#ifdef ENABLE_CGAL
typedef CGAL::Exact_predicates_inexact_constructions_kernel           K;
typedef CGAL::Delaunay_triangulation_2<K>                             Delaunay_triangulation;
typedef CGAL::Interpolation_traits_2<K>                               Traits;
typedef K::FT                                                         Coord_type;
typedef K::Point_2                                                    Point;
typedef CGAL::Data_access<std::map<Point, Coord_type, K::Less_xy_2> > Value_access;
#endif // ENABLE_CGAL

/*-------------------------------- LOCAL VARIABLES --------------------------------*/

static int vec_counter = 0;

#ifdef ENABLE_PNG
static png_byte **rows = NULL;
#endif // ENABLE_PNG

#ifdef ENABLE_CGAL
static int tex_counter = 0;
static unsigned char *image = NULL;
#endif // ENABLE_CGAL

/*-------------------------------- EXPORT FUNCTIONS --------------------------------*/

void export_vector(int chemical)
{
	char filename[256];
	sprintf(filename, "vec-%02d.svg", vec_counter++);
	FILE *svg = fopen(filename, "w");
	if (svg == NULL) {
		std::cerr << "svg writing failed!\n";
		PAUSE_AND_EXIT;
	}

	// output header
	fprintf(svg, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	fprintf(svg, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" stroke=\"black\" stroke-width=\"0.1\" ");
	fprintf(svg, "width=\"%g\" height=\"%g\">\n", statistics.cell_xmax - statistics.cell_xmin + 2,
			statistics.cell_ymax - statistics.cell_ymin + 2);
	fprintf(svg, "  <g transform=\"translate(%g,%g) scale(1,-1)\">\n", -statistics.cell_xmin + 1, statistics.cell_ymax + 1);

	// prepare colormap
    colormap_set_limits(statistics.chem_min[chemical], statistics.chem_max[chemical]);

    // output all cells
	for (int i = 0; i < simulation.n_cells; i++) {
		Cell& curr_cell = simulation.curr_cells[i];
		float x = curr_cell.x;
		float y = curr_cell.y;
		float v = curr_cell.conc[chemical];

		float *color = colormap_lookup(v);
		fprintf(svg, "    <circle cx=\"%g\" cy=\"%g\" r=\"1\" ", x, y);
		fprintf(svg, "fill=\"#%02x%02x%02x\" />\n", (int) (color[0] * 255), (int) (color[1] * 255), (int) (color[2] * 255));
	}

	fprintf(svg, "  </g>\n");
	fprintf(svg, "</svg>\n");
	fclose(svg);
}

// adapted from http://stackoverflow.com/questions/3191978/how-to-use-glut-opengl-to-render-to-a-file

#ifdef ENABLE_PNG
void export_png(const char *filename, int width, int height, unsigned char *pixels)
{
    rows = (png_byte **) realloc(rows, height * sizeof(png_byte *));
    for (int i = 0; i < height; i++) {
        rows[height - i - 1] = &pixels[i * width * 3];
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
    	std::cerr << "png writing failed!\n";
    	PAUSE_AND_EXIT;
    }
    png_infop info = png_create_info_struct(png);
    if (!info) {
    	std::cerr << "png writing failed!\n";
    	PAUSE_AND_EXIT;
    }
    if (setjmp(png_jmpbuf(png))) {
    	std::cerr << "png writing failed!\n";
    	PAUSE_AND_EXIT;
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
#endif // ENABLE_PNG

#ifdef ENABLE_CGAL
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

    float xrange = statistics.cell_xmax - statistics.cell_xmin;
    float yrange = statistics.cell_ymax - statistics.cell_ymin;
    float xmin = statistics.cell_xmin;
    float ymin = statistics.cell_ymin;
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

	// output texture
	char filename[256];
	sprintf(filename, "tex-%02d.png", tex_counter++);
	export_png(filename, width, height, image);
}
#endif // ENABLE_CGAL
