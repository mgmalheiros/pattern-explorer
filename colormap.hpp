#ifndef COLORMAP_HPP
#define COLORMAP_HPP

/*-------------------------------- TYPE DEFINITIONS --------------------------------*/

enum Colormap {HEAT = 0, STRIPED, GRADIENT};

/*-------------------------------- EXPORTED FUNCTIONS --------------------------------*/

void colormap_init();

long int colormap_convert_color(const char *s);

void colormap_use_color(int i, const char *s);
void colormap_use_rgb(int i, int r, int g, int b);

void colormap_generate();

void colormap_select(Colormap c);
Colormap colormap_get_selected();

void colormap_set_limits(float min, float max);

float* colormap_lookup(float val);

#endif // COLORMAP_HPP
