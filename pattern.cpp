/*-------------------------------- INCLUDES --------------------------------*/

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>

//#include <time.h>

#include <GL/freeglut.h>
#include <AntTweakBar.h>

#include <glm/gtc/matrix_transform.hpp>

#include "colormap.hpp"
#include "export.hpp"
#include "nns_base.hpp"
#include "parser.hpp"
#include "simulation.hpp"
#include "types.hpp"

/*-------------------------------- IMPORTED VARIABLES --------------------------------*/

extern NNS *nns;
extern Simulation simulation;
extern Statistics statistics;

//extern float time_draw;

/*-------------------------------- LOCAL TYPES --------------------------------*/

enum CellExhibition {OCTOGON = 0, SQUARE, HEXAGON_INSIDE, HEXAGON_OUTSIDE, CIRCLE};

/*-------------------------------- LOCAL FUNCTIONS --------------------------------*/

void graphics_done();
void bar_show(bool b);

/*-------------------------------- LOCAL VARIABLES --------------------------------*/

static CellExhibition cell_ex = OCTOGON;

static bool  move_mode = false;
static bool  offset_mode = false;
static float offset_x = 0, offset_y = 0;
static bool  pick_mode = false;
static int   window_w = 640, window_h = 640;
static int   window_x = 680, window_y = 0;

static int   value_active = 0;

static Colormap map_active = HEAT;

static bool show_bar = true;
static bool show_domain = false;
static bool show_mirrors = false;
static bool show_neighbors = false;
static bool show_polarity = false;

static CellId    picked_cell_id = (CellId) -1;
static glm::vec3 picked_point;
static glm::vec3 offset_point;

static TwBar *bar = NULL;
static int    bar_w = 160;

static int out_counter = 0;
static GLubyte *pixels = NULL;

/*-------------------------------- LOCAL UTILITY FUNCTIONS --------------------------------*/

static char *concat(const char *s, int n)
{
	char tmp[100];
	snprintf(tmp, sizeof(tmp), "%s%d", s, n);
	return strdup(tmp);
}

static char *concat(int n1, const char *s, int n2)
{
	char tmp[100];
	snprintf(tmp, sizeof(tmp), "%d%s%d", n1, s, n2);
	return strdup(tmp);
}

static char *concat(const char *s1, const char *s2)
{
	char tmp[100];
	snprintf(tmp, sizeof(tmp), "%s%s", s1, s2);
	return strdup(tmp);
}

/*-------------------------------- SCREEN CAPTURE FUNCTIONS --------------------------------*/

void take_snapshot()
{
	// NOTE: glReadPixels() often fails after the window has been resized
    pixels = (GLubyte *) realloc(pixels, window_w * window_h * 3 * sizeof(GLubyte));
    glReadPixels(0, 0, window_w, window_h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	char filename[256];
	sprintf(filename, "out-%02d.png", out_counter);
	export_png(filename, window_w, window_h, pixels);
	out_counter++;
}

/*-------------------------------- GRAPHICS FUNCTIONS --------------------------------*/

void display()
{
//	struct timespec t0, t1;
//	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);

//	if (vector)
//	{
//		vector_open_file();
//	}

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glScalef(simulation.zoom_level, simulation.zoom_level, 1);
    glTranslatef(offset_x, offset_y, 0);

#ifndef NNS_PRECISION
    colormap_set_limits(statistics.chem_min[value_active], statistics.chem_max[value_active]);
#else
    colormap_set_limits(0, statistics.error_max);
#endif // NNS_PRECISION

    // draw cells
	for (int i = 0; i < simulation.n_cells; i++) {
		Cell& curr_cell = simulation.curr_cells[i];
		float x = curr_cell.x;
		float y = curr_cell.y;
#ifndef NNS_PRECISION
		glColor3fv(colormap_lookup(curr_cell.conc[value_active]));
#else
		glColor3fv(colormap_lookup(curr_cell.error));
#endif // NNS_PRECISION

//		if (vector) {
//			vector_draw_cell(x, y, r);
//		}

		if (show_neighbors && (curr_cell.marker == true)) {
			glColor3f(1, 1, 1);
		}

		if (cell_ex == OCTOGON) {
			// draw a filled octogon
			glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x + 1,      y);
			glVertex2f(x + 0.7071, y + 0.7071);
			glVertex2f(x,          y + 1);
			glVertex2f(x - 0.7071, y + 0.7071);
			glVertex2f(x - 1,      y);
			glVertex2f(x - 0.7071, y - 0.7071);
			glVertex2f(x,          y - 1);
			glVertex2f(x + 0.7071, y - 0.7071);
			glEnd();
		}
		else if (cell_ex == SQUARE) {
			// draw a filled square
			glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x + 1, y + 1);
			glVertex2f(x - 1, y + 1);
			glVertex2f(x - 1, y - 1);
			glVertex2f(x + 1, y - 1);
			glEnd();
		}
		else if (cell_ex == HEXAGON_INSIDE) {
			// draw a filled inscribed hexagon
			glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x        , y - 1);
			glVertex2f(x + 0.866, y - 0.5);
			glVertex2f(x + 0.866, y + 0.5);
			glVertex2f(x        , y + 1);
			glVertex2f(x - 0.866, y + 0.5);
			glVertex2f(x - 0.866, y - 0.5);
			glEnd();
		}
		else if (cell_ex == HEXAGON_OUTSIDE) {
			// draw a filled circumscribed hexagon
			glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x    , y - 1.1547);
			glVertex2f(x + 1, y - 0.577);
			glVertex2f(x + 1, y + 0.577);
			glVertex2f(x    , y + 1.1547);
			glVertex2f(x - 1, y + 0.577);
			glVertex2f(x - 1, y - 0.577);
			glEnd();
		}
		else {
			// draw a filled circle
			glBegin(GL_TRIANGLE_FAN);
			for (int a = 0; a < 18; a++) {
				glVertex2f(x + cosf(M_PI * a / 9), y + sinf(M_PI * a / 9));
			}
			glEnd();
		}
	}

//	if (vector)
//	{
//		vector_close_file();
//		vector = false;
//	}

	// draw mirror pairs
	if (show_mirrors) {
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for (int i = 0; i < (int) simulation.mirror_list.size(); i++) {
			int first = simulation.mirror_list[i].first;
			int second = simulation.mirror_list[i].second;
			if (picked_cell_id == first || picked_cell_id == second) {
				glColor3f(1, 1, 1);
			}
			glVertex2f(simulation.curr_cells[first].x, simulation.curr_cells[first].y);
			glVertex2f(simulation.curr_cells[second].x, simulation.curr_cells[second].y);
			if (picked_cell_id == first || picked_cell_id == second) {
				glColor3f(1, 0, 0);
			}
		}
		glEnd();
	}

	// draw polarity vectors
	if (show_polarity) {
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		for (int i = 0; i < simulation.n_cells; i++) {
			glVertex2f(simulation.curr_cells[i].x, simulation.curr_cells[i].y);
			glVertex2f(simulation.curr_cells[i].x + simulation.curr_cells[i].polarity_x,
					   simulation.curr_cells[i].y + simulation.curr_cells[i].polarity_y);
		}
		glEnd();
	}

    // draw influence range for selected cell
    if (picked_cell_id != -1) {
    	Cell& picked_cell = simulation.curr_cells[picked_cell_id];
    	float x = picked_cell.x;
    	float y = picked_cell.y;
    	glColor3f(1, 1, 1);

    	// draw an octagonal outline
    	glBegin(GL_LINE_LOOP);
		glVertex2f(x + INFLUENCE_RANGE,          y);
		glVertex2f(x + INFLUENCE_RANGE * 0.7071, y + INFLUENCE_RANGE * 0.7071);
		glVertex2f(x,                            y + INFLUENCE_RANGE);
		glVertex2f(x - INFLUENCE_RANGE * 0.7071, y + INFLUENCE_RANGE * 0.7071);
		glVertex2f(x - INFLUENCE_RANGE,          y);
		glVertex2f(x - INFLUENCE_RANGE * 0.7071, y - INFLUENCE_RANGE * 0.7071);
		glVertex2f(x,                            y - INFLUENCE_RANGE);
		glVertex2f(x + INFLUENCE_RANGE * 0.7071, y - INFLUENCE_RANGE * 0.7071);
    	glEnd();
    }

    // draw domain limits
    if (show_domain) {
    	glColor3f(0.2, 0.2, 0.2);
    	glBegin(GL_LINE_LOOP);
    	glVertex2f(simulation.domain_xmax, simulation.domain_ymax);
    	glVertex2f(simulation.domain_xmin, simulation.domain_ymax);
    	glVertex2f(simulation.domain_xmin, simulation.domain_ymin);
    	glVertex2f(simulation.domain_xmax, simulation.domain_ymin);
    	glEnd();
    }

    // draw tweak bar
    TwDraw();

    glutSwapBuffers();

//	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
//  time_draw += (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) * 0.000001;
}

void reshape(int w, int h)
{
	window_w = w;
	window_h = h;
    TwWindowSize(window_w, window_h);
    TwSetParam(bar, NULL, "size", TW_PARAM_CSTRING, 1, concat(bar_w, " ", window_h));

    if (show_bar) {
    	w -= bar_w;
    }
    glViewport((show_bar) ? bar_w : 0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h) {
        glOrtho(-100.0 * w / h, 100.0 * w / h, -100.0, 100.0, -100.0, 100.0);
    }
    else {
        glOrtho(-100.0, 100.0, -100.0 * h / w, 100.0 * h / w, -100.0, 100.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

glm::vec3 unproject(int x, int y)
{
	int w = (show_bar) ? window_w - bar_w : window_w;
	int h = window_h;

	glm::vec3 point(x, h - y, 0);

	glm::mat4 modelview(1);
	modelview = glm::scale(modelview, glm::vec3(simulation.zoom_level, simulation.zoom_level, 1));
	modelview = glm::translate(modelview, glm::vec3(offset_x, offset_y, 0));

	glm::mat4 projection;
	if (w > h) {
		projection = glm::ortho(-100.0 * w / h, 100.0 * w / h, -100.0, 100.0, -100.0, 100.0);
	}
	else {
		projection = glm::ortho(-100.0, 100.0, -100.0 * h / w, 100.0 * h / w, -100.0, 100.0);
	}

	glm::vec4 viewport((show_bar) ? bar_w : 0, 0, w, h);
	return glm::unProject(point, modelview, projection, viewport);
}

void center_view()
{
	int w = (show_bar) ? window_w - bar_w : window_w;
	int h = window_h;
	offset_x = - (statistics.cell_xmax + statistics.cell_xmin) / 2;
	offset_y = - (statistics.cell_ymax + statistics.cell_ymin) / 2;
	if (w > h) {
		simulation.zoom_level = 200 / (statistics.cell_ymax - statistics.cell_ymin + 2); // 2 * cell radius
	}
	else {
		simulation.zoom_level = 200 / (statistics.cell_xmax - statistics.cell_xmin + 2); // 2 * cell radius
	}
}

/*-------------------------------- GLUT CALLBACK FUNCTIONS --------------------------------*/

void idle()
{
	if (simulation.is_running) {
		if (simulation.snap_at.size()) {
			if (simulation.snap_at[0] == simulation.iteration || simulation.is_stable) {
				std::cout << "gui: snap at " << simulation.iteration << "\n";
				take_snapshot();
				simulation.snap_at.erase(simulation.snap_at.begin());

				// there are no snapshots left
				if (simulation.snap_at.size() == 0 || simulation.is_stable) {
					// exit now if requested
					if (simulation.exit_at_end) {
			        	graphics_done();
			            simulation_done();
						exit(0);
					}

					// restore bar and pause simulation
					bar_show(true);
					reshape(window_w, window_h);
					simulation.is_running = false;
				}
			}
			else {
				//std::cout << "run " << simulation.snap_at[0] - simulation.iteration << " steps\n";
				simulation_run(simulation.snap_at[0] - simulation.iteration);
			}
		}
		else {
			// jump to next iteration that is multiple of 100
			simulation_run(100 - simulation.iteration % 100);
		}
	}
	TwRefreshBar(bar);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if (TwEventKeyboardGLUT(key, x, y)) {
		glutPostRedisplay();
		return;
	}

    switch (key) {
        case ' ': // SPACE - run single step
            simulation.is_running = false;
            simulation_run(1);
            TwRefreshBar(bar);
            glutPostRedisplay();
            break;
        case 9: // TAB - run to multiple of 50 steps
            simulation.is_running = false;
            simulation_run(50 - simulation.iteration % 50);
            TwRefreshBar(bar);
            glutPostRedisplay();
            break;
        case 27:  // ESC - quit
        	graphics_done();
            simulation_done();
            exit(0);
            break;
        case 'a': // change active chemical
        	value_active = (value_active + 1) % simulation.n_chemicals;
            TwRefreshBar(bar);
            glutPostRedisplay();
            break;
        case 'b': // show or hide bar
        	bar_show(show_bar ^ 1);
        	reshape(window_w, window_h);
            glutPostRedisplay();
            break;
        case 'c': // center and fit view
        	center_view();
        	std::cout << "zoom level " << std::setprecision(4) << simulation.zoom_level << '\n';
            glutPostRedisplay();
            break;
        case 'd': // show or hide domain
        	show_domain ^= 1;
            glutPostRedisplay();
            break;
//        case 'e': // export cell data
//        	FILE *f;
//        	f = fopen("points.txt", "w");
//        	for (int i = 0; i < cell_count; i++) {
//        		fprintf(f, "%g %g\n", simulation.position_array[i].x, -simulation.position_array[i].y);
//        	}
//        	fclose(f);
//        	printf("points saved!\n");
//        	break;
        case 'i': // show or hide mirror pairs
        	show_mirrors ^= 1;
            glutPostRedisplay();
            break;
        case 'l': // load colormap definitions
        	colormap_init();
        	parser_load_colormap();
        	colormap_generate();
        	map_active = colormap_get_selected();
            TwRefreshBar(bar);
            glutPostRedisplay();
            break;
        case 'm': // change active colormap
        	map_active = (Colormap) ((map_active + 1) % 3); // was 5
            colormap_select(map_active);
            TwRefreshBar(bar);
            glutPostRedisplay();
            break;
        case 'n': // show or hide neighbors for picked cell
        	show_neighbors ^= 1;
            glutPostRedisplay();
            break;
        case 'o': // output image
        	take_snapshot();
        	std::cout << "gui: snapshot saved\n";
            break;
        case 'p': // show or hide polarity vectors
        	show_polarity ^= 1;
            glutPostRedisplay();
            break;
        case 's': // start/stop
            simulation.is_running = ! simulation.is_running;
            TwRefreshBar(bar);
            break;
        case 't': // output high-quality interpolated texture
        	std::cout << "gui: saving " << simulation.texture_width << " by " << simulation.texture_height << " texture...\n";
        	export_texture(value_active, simulation.texture_width, simulation.texture_height);
        	std::cout << "gui: ... done\n";
            break;
//        case 'w': // output high-quality interpolated wrapped texture
//        	std::cout << "gui: saving " << simulation.texture_width << " by " << simulation.texture_height << " wrapped texture...\n";
//        	export_texture_wrap(value_active, simulation.texture_width, simulation.texture_height, 64, 64);
//        	std::cout << "gui: ... done\n";
            break;
//        case 'v': // generate vector output
//        	vector = true;
//            glutPostRedisplay();
//            break;
        case 'x': // change cell exhibition type
        	cell_ex = (CellExhibition) ((cell_ex + 1) % 5);
            glutPostRedisplay();
            break;
    }
}

void mouse_click(int button, int state, int x, int y)
{
	if (TwEventMouseButtonGLUT(button, state, x, y)) {
		glutPostRedisplay();
		return;
	}

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        pick_mode = true;
        glutSetCursor(GLUT_CURSOR_INFO);
        picked_point = unproject(x, y);
    	//std::cout << "picked at " << x << "," << y << " corresponds to " << picked_point.x << "," << picked_point.y << '\n';
    	CellId id = nns->locate_nearest(picked_point.x, picked_point.y);
		picked_cell_id = id;
		TwDefine("Simulation/Cell visible=true");
    	simulation.tracked_id = picked_cell_id;
        TwRefreshBar(bar);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        pick_mode = false;
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        offset_mode = true;
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        offset_point = unproject(x, y);
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
        offset_mode = false;
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        move_mode = true;
        glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
        picked_point = unproject(x, y);
    	//std::cout << "picked at " << x << "," << y << " corresponds to " << picked_point.x << "," << picked_point.y << '\n';
        CellId id = nns->locate_nearest(picked_point.x, picked_point.y);
        float x = simulation.curr_cells[id].x;
        float y = simulation.curr_cells[id].y;
    	if ((picked_point.x - x) * (picked_point.x - x) + (picked_point.y - y) * (picked_point.y - y) <= 1) {
    		//std::cout << "picked cell at " << x << "," << y << " id=" << id << '\n';
    		picked_cell_id = id;
    		TwDefine("Simulation/Cell visible=true");
    	}
    	else {
    		//std::cout << "no cell picked\n";
    		picked_cell_id = -1;
    		TwDefine("Simulation/Cell visible=false");
    	}
    	simulation.tracked_id = picked_cell_id;
        TwRefreshBar(bar);
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        move_mode = false;
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
    else if (button == 3 && state == GLUT_UP && simulation.zoom_level < 8.0) { // scroll button up
    	simulation.zoom_level += 0.1;
        glutPostRedisplay();
    }
    else if (button == 4 && state == GLUT_UP && simulation.zoom_level > 0.2) { // scroll button down
    	simulation.zoom_level -= 0.1;
        glutPostRedisplay();
    }
}

void mouse_motion(int x, int y)
{
	if (TwEventMouseMotionGLUT(x, y)) {
	    glutPostRedisplay();
		return;
	}

    if (pick_mode) {
        glutSetCursor(GLUT_CURSOR_INFO);
    }
    else if (offset_mode) {
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    	glm::vec3 point = unproject(x, y);
        offset_x += point.x - offset_point.x;
        offset_y += point.y - offset_point.y;
        glutPostRedisplay();
    }
    else if (move_mode) {
        glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
    	picked_point = unproject(x, y);
    	if (picked_cell_id != -1) {
    		simulation.curr_cells[picked_cell_id].x = picked_point.x;
    		simulation.curr_cells[picked_cell_id].y = picked_point.y;
    	    nns->update_position(picked_cell_id, picked_point.x, picked_point.y);
    	}
        glutPostRedisplay();
    }
}

void special(int key, int x, int y)
{
	if (TwEventSpecialGLUT(key, x, y)) {
	    glutPostRedisplay();
	    return;
	}
    switch (key) {
        case GLUT_KEY_LEFT:
        	offset_x += 10;
        	break;
        case GLUT_KEY_RIGHT:
        	offset_x -= 10;
        	break;
        case GLUT_KEY_UP:
        	offset_y -= 10;
        	break;
        case GLUT_KEY_DOWN:
        	offset_y += 10;
        	break;
    }
}

/*-------------------------------- TWEAK BAR FUNCTIONS --------------------------------*/

enum CellAttributes {CELL_ID, CELL_BIRTH, CELL_AGE, CELL_NEIG, CELL_X, CELL_Y, CELL_PX, CELL_PY,
	                 CONC_0, CONC_1, CONC_2, CONC_3, CONC_4, CONC_5, CONC_6, CONC_7, CONC_8, CONC_9,
	                 DIFF_0, DIFF_1, DIFF_2, DIFF_3, DIFF_4, DIFF_5, DIFF_6, DIFF_7, DIFF_8, DIFF_9};
int ca[] =          {CELL_ID, CELL_BIRTH, CELL_AGE, CELL_NEIG, CELL_X, CELL_Y, CELL_PX, CELL_PY,
                     CONC_0, CONC_1, CONC_2, CONC_3, CONC_4, CONC_5, CONC_6, CONC_7, CONC_8, CONC_9,
                     DIFF_0, DIFF_1, DIFF_2, DIFF_3, DIFF_4, DIFF_5, DIFF_6, DIFF_7, DIFF_8, DIFF_9};

void TW_CALL set_cell_state(const void *value, void *client_data)
{
	int attrib = *(static_cast<int *> (client_data));

	if (attrib == CELL_ID) {
		const int id = *(static_cast<const int *> (value));
		if (id >= 0 && id < simulation.n_cells) {
			picked_cell_id = id;
		}
		return;
	}

	if (picked_cell_id == -1) {
		std::cout << "this should not happen in set_cell_state...\n";
		return;
	}

	if (attrib == CELL_BIRTH) {
		const int val = *(static_cast<const int *> (value));
		simulation.curr_cells[picked_cell_id].birth = val;
		return;
	}

	if (attrib == CELL_AGE) { // should be read-only
		return;
	}

	if (attrib == CELL_NEIG) { // should be read-only
		return;
	}
	const float val = *(static_cast<const float *> (value));
	if (attrib == CELL_X) {
		simulation.curr_cells[picked_cell_id].x = val;
	    nns->update_position(picked_cell_id, val, simulation.curr_cells[picked_cell_id].y);
	}
	else if (attrib == CELL_Y) {
		simulation.curr_cells[picked_cell_id].y = val;
	    nns->update_position(picked_cell_id, simulation.curr_cells[picked_cell_id].x, val);
	}
	else if (attrib == CELL_PX) {
		simulation.curr_cells[picked_cell_id].polarity_x = val;
	}
	else if (attrib == CELL_PY) {
		simulation.curr_cells[picked_cell_id].polarity_y = val;
	}
	else if (attrib >= CONC_0 && attrib <= CONC_9) {
		int i = attrib - CONC_0;
		simulation.curr_cells[picked_cell_id].conc[i] = val;
	}
	else if (attrib >= DIFF_0 && attrib <= DIFF_9) {
		int i = attrib - DIFF_0;
		simulation.curr_cells[picked_cell_id].diff[i] = val;
	}
}

void TW_CALL get_cell_state(void *value, void *client_data)
{
	int attrib = *(static_cast<int *> (client_data));

	if (attrib == CELL_ID) {
		*(static_cast<int *> (value)) = picked_cell_id;
		return;
	}

	if (picked_cell_id == -1) {
		std::cout << "this should not happen in get_cell_state...\n";
		return;
	}

	if (attrib == CELL_BIRTH) {
		int val = simulation.curr_cells[picked_cell_id].birth;
		*(static_cast<int *> (value)) = val;
		return;
	}

	if (attrib == CELL_AGE) {
		int val = simulation.iteration - simulation.curr_cells[picked_cell_id].birth;
		*(static_cast<int *> (value)) = val;
		return;
	}

	if (attrib == CELL_NEIG) {
		int val = simulation.curr_cells[picked_cell_id].neighbors;
		*(static_cast<int *> (value)) = val;
		return;
	}

	float val = 0;
	if (attrib == CELL_X) {
		val = simulation.curr_cells[picked_cell_id].x;
	}
	else if (attrib == CELL_Y) {
		val = simulation.curr_cells[picked_cell_id].y;
	}
	else if (attrib == CELL_PX) {
		val = simulation.curr_cells[picked_cell_id].polarity_x;
	}
	else if (attrib == CELL_PY) {
		val = simulation.curr_cells[picked_cell_id].polarity_y;
	}
	else if (attrib >= CONC_0 && attrib <= CONC_9) {
		int i = attrib - CONC_0;
		val = simulation.curr_cells[picked_cell_id].conc[i];
	}
	else if (attrib >= DIFF_0 && attrib <= DIFF_9) {
		int i = attrib - DIFF_0;
		val = simulation.curr_cells[picked_cell_id].diff[i];
	}
	*(static_cast<float *> (value)) = val;
}

void bar_init()
{
    TwInit(TW_OPENGL, NULL);
    TwWindowSize(window_w, window_h);
    TwGLUTModifiersFunc(glutGetModifiers);

    bar = TwNewBar("Simulation");
    TwDefine("Simulation refresh=0.5 position='0 0' valueswidth=fit iconifiable=false movable=false resizable=false");
    TwSetParam(bar, NULL, "size", TW_PARAM_CSTRING, 1, concat(bar_w, " ", window_h));

    /*---------------- main variables --------------*/

    TwAddVarRW(bar, "status",    TW_TYPE_BOOLCPP, &simulation.is_running, "true='running' false='paused'");
    TwAddVarRO(bar, "iteration", TW_TYPE_INT32,   &simulation.iteration, NULL);

    /*---------------- statistics group --------------*/

    TwAddVarRO(bar, "cells",      TW_TYPE_INT32, &simulation.n_cells,    "group='Statistics'");
    //TwAddVarRO(bar, "last_birth", TW_TYPE_INT32, &statistics.last_birth, "group='Statistics' label='last birth'");

    TwAddVarRO(bar, "cell_xmax",  TW_TYPE_FLOAT, &statistics.cell_xmax, "group='Geometry' precision=1 label='x max'");
    TwAddVarRO(bar, "cell_xmin",  TW_TYPE_FLOAT, &statistics.cell_xmin, "group='Geometry' precision=1 label='x min'");
    TwAddVarRO(bar, "cell_ymax",  TW_TYPE_FLOAT, &statistics.cell_ymax, "group='Geometry' precision=1 label='y max'");
    TwAddVarRO(bar, "cell_ymin",  TW_TYPE_FLOAT, &statistics.cell_ymin, "group='Geometry' precision=1 label='y min'");
    TwAddVarRO(bar, "cell_nmax",  TW_TYPE_FLOAT, &statistics.cell_nmax, "group='Geometry' precision=0 label='n max'");
    TwAddVarRO(bar, "cell_navg",  TW_TYPE_FLOAT, &statistics.cell_navg, "group='Geometry' precision=2 label='n avg'");
    TwAddVarRO(bar, "cell_nmin",  TW_TYPE_FLOAT, &statistics.cell_nmin, "group='Geometry' precision=0 label='n min'");
    TwDefine("Simulation/Geometry opened=false");
    TwDefine("Simulation/Geometry group='Statistics'");

    int n_chemicals = (int) simulation.n_chemicals;
    for (int ch = 0; ch < n_chemicals; ch++) {
    	char *name  = concat("chem_max", ch);
    	TwAddVarRO(bar, name, TW_TYPE_FLOAT, &statistics.chem_max[ch], "group='Chemicals' precision=4");
    	TwSetParam(bar, name, "label", TW_PARAM_CSTRING, 1, concat(simulation.chemicals[ch].name.c_str(), " max"));
    	name  = concat("chem_min", ch);
    	TwAddVarRO(bar, name, TW_TYPE_FLOAT, &statistics.chem_min[ch], "group='Chemicals' precision=4");
    	TwSetParam(bar, name, "label", TW_PARAM_CSTRING, 1, concat(simulation.chemicals[ch].name.c_str(), " min"));
    }
    TwDefine("Simulation/Chemicals opened=true");
    TwDefine("Simulation/Chemicals group='Statistics'");

    /*---------------- display group --------------*/

    TwEnumVal val_enum[n_chemicals];
    for (int v = 0; v < n_chemicals; v++) {
    	val_enum[v].Value = v;
    	val_enum[v].Label = simulation.chemicals[v].name.c_str();
    }
    TwType values = TwDefineEnum("Values", val_enum, n_chemicals);
    TwAddVarRW(bar, "active", values, &value_active, "group='Display' label='value'");

    TwEnumVal map_enum[3] = {{HEAT, "heat"}, {STRIPED, "striped"}, {GRADIENT, "gradient"}};
    TwType maps = TwDefineEnum("Maps", map_enum, 3);
    TwAddVarRW(bar, "map", maps, &map_active, "group='Display' label='map'");

    TwDefine("Simulation/Display opened=true");

    /*---------------- cell group --------------*/

    TwAddVarCB(bar, "id",    TW_TYPE_INT32, set_cell_state, get_cell_state, &ca[CELL_ID],    "group='Cell'");
    TwAddVarCB(bar, "birth", TW_TYPE_INT32, set_cell_state, get_cell_state, &ca[CELL_BIRTH], "group='Cell'");
    TwAddVarCB(bar, "age",   TW_TYPE_INT32, set_cell_state, get_cell_state, &ca[CELL_AGE],   "group='Cell' readonly=true");
    TwAddVarCB(bar, "n",     TW_TYPE_INT32, set_cell_state, get_cell_state, &ca[CELL_NEIG],  "group='Cell' readonly=true");
    TwAddVarCB(bar, "x",     TW_TYPE_FLOAT, set_cell_state, get_cell_state, &ca[CELL_X],     "group='Cell' precision=1");
    TwAddVarCB(bar, "y",     TW_TYPE_FLOAT, set_cell_state, get_cell_state, &ca[CELL_Y],     "group='Cell' precision=1");
    TwAddVarCB(bar, "px",    TW_TYPE_FLOAT, set_cell_state, get_cell_state, &ca[CELL_PX],    "group='Cell' precision=1");
    TwAddVarCB(bar, "py",    TW_TYPE_FLOAT, set_cell_state, get_cell_state, &ca[CELL_PY],    "group='Cell' precision=1");
    // TODO: add "fixed" state?
    for (int ch = 0; ch < (int) simulation.n_chemicals; ch++) {
    	char *name  = concat("conc", ch);
        TwAddVarCB(bar, name, TW_TYPE_FLOAT, set_cell_state, get_cell_state, &ca[CONC_0 + ch], "group='Cell' precision=4");
    	TwSetParam(bar, name, "label", TW_PARAM_CSTRING, 1, concat(simulation.chemicals[ch].name.c_str(), " conc"));
    	name  = concat("diff", ch);
        TwAddVarCB(bar, name, TW_TYPE_FLOAT, set_cell_state, get_cell_state, &ca[DIFF_0 + ch], "group='Cell' precision=4");
    	TwSetParam(bar, name, "label", TW_PARAM_CSTRING, 1, concat(simulation.chemicals[ch].name.c_str(), " diff"));
    }
    TwDefine("Simulation/Cell opened=true");
	TwDefine("Simulation/Cell visible=false");

	TwDefine("TW_HELP visible=false");
}

void bar_show(bool b)
{
	if (b) {
		TwDefine("Simulation visible=true");
	}
	else {
		TwDefine("Simulation visible=false");
	}
	show_bar = b;
}

/*-------------------------------- MAIN FUNCTIONS --------------------------------*/

void graphics_init(int *pargc, char *argv[])
{
    glutInit(pargc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(window_x, window_y);
    glutInitWindowSize(window_w, window_h);
    glutCreateWindow("pattern explorer");

    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse_click);
    glutMotionFunc(mouse_motion);
    glutPassiveMotionFunc((GLUTmousemotionfun) TwEventMouseMotionGLUT);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_MODELVIEW);

    if (simulation.zoom_level == 0) {
    	center_view();
    }
    bar_init();

    // prepare for taking snapshots
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    map_active = colormap_get_selected();
    
	// hide bar and start simulation if there are snapshots to be taken
	if (simulation.snap_at.size() != 0) {
		bar_show(false);
		simulation.is_running = true;
	}
	else  {
		bar_show(true);
		simulation.is_running = false;
	}
}

void graphics_loop()
{
	glutMainLoop();
}

void graphics_done()
{
	TwTerminate();
	free(pixels);
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
    	std::cout << "usage: pattern [OPTION] ... FILE.pat\n";
    	std::cout << "  --ss         force use of spatial sorting\n";
    	std::cout << "  --kd         force use of k-d tree\n";
    	std::cout << "  --detect     exit as soon as stability is detected\n";
    	std::cout << "  --oct        draw each cell as an octogon (default)\n";
    	std::cout << "  --sqr        draw each cell as a square\n";
    	std::cout << "  --hex_in     draw each cell as an inscribed hexagon\n";
    	std::cout << "  --hex_out    draw each cell as a circumscribed hexagon\n";
    	std::cout << "  --circle     draw each cell as a circle\n";
    	std::cout << '\n';
    	exit(1);
    }
    argv++; argc--;

    bool detect = false;
    NNSChoice nns_choice = AUTO;
    while ((*argv)[0] == '-') {
    	if (strcmp(*argv, "--ss") == 0) {
    		nns_choice = SPATIAL_SORTING;
    	}
    	else if (strcmp(*argv, "--kd") == 0) {
    		nns_choice = KD_TREE;
    	}
    	else if (strcmp(*argv, "--detect") == 0) {
    		detect = true;;
    	}
    	else if (strcmp(*argv, "--oct") == 0) {
    		cell_ex = OCTOGON;
    	}
    	else if (strcmp(*argv, "--sqr") == 0) {
    		cell_ex = SQUARE;
    	}
    	else if (strcmp(*argv, "--hex-in") == 0) {
    		cell_ex = HEXAGON_INSIDE;
    	}
    	else if (strcmp(*argv, "--hex-out") == 0) {
    		cell_ex = HEXAGON_OUTSIDE;
    	}
    	else if (strcmp(*argv, "--circle") == 0) {
    		cell_ex = CIRCLE;
    	}
    	else {
    		std::cout << "unknown option '" << *argv << "'\n";
    		exit(1);
    	}
    	argv++; argc--;
    }

	parser_init(*argv);
	parser_load_pattern();

	colormap_init();
	parser_load_colormap();
	colormap_generate();

	simulation_init(nns_choice, detect);

    graphics_init(&argc, argv);
    graphics_loop(); // never returns

    return 0;
}
