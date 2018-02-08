#ifndef EXPORT_HPP
#define EXPORT_HPP

/*-------------------------------- EXPORTED FUNCTIONS --------------------------------*/

void export_vector(int chemical = 0);

#ifdef ENABLE_PNG
void export_png(const char *filename, int width, int height, unsigned char *pixels);
#endif // ENABLE_PNG

#ifdef ENABLE_CGAL
void export_texture(int chemical = 0, int width = 256, int height = 256);
#endif // ENABLE_CGAL

#endif // EXPORT_HPP
