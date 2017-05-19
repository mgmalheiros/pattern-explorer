#ifndef EXPORT_HPP
#define EXPORT_HPP

/*-------------------------------- EXPORTED FUNCTIONS --------------------------------*/

void export_png(const char *filename, int width, int height, unsigned char *pixels);

void export_texture(int chemical = 0, int width = 256, int height = 256);

//void export_texture_wrap(int chemical, int width, int height, int nns_dim_x, int nns_dim_y);

void export_vector(int chemical = 0);

#endif // EXPORT_HPP
