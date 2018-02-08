# Optional dependencies

OPT_PNG = #-DENABLE_PNG -I...
LIB_PNG = #-lpng -L...

OPT_CGAL = #-DENABLE_CGAL -I...
LIB_CGAL = #-lCGAL -lboost_system -lgmp -L...

# Required dependencies

OPT_ATB = #-I...
LIB_ATB = -lAntTweakBar #-L...

OPT_GLM = #-I...

OPT_OPENGL = #-I...
LIB_OPENGL = -lglut -lGL #-L...

# General settings

OPT = -O2 -Wall -Wextra
LIB = -m

# Programs

all: pattern offline simple

pattern: colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o pattern.o simulation.o
	g++  colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o pattern.o simulation.o $(LIB_PNG) $(LIB_CGAL) $(LIB_ATB) $(LIB_OPENGL) $(LIB) -o pattern

pattern.o: colormap.hpp export.hpp nns_base.hpp parser.hpp simulation.hpp types.hpp pattern.cpp
	g++ $(OPT) $(OPT_PNG) $(OPT_CGAL) $(OPT_ATB) $(OPT_GLM) $(OPT_OPENGL) -c pattern.cpp

offline: colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o offline.o simulation.o
	g++  colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o offline.o simulation.o $(LIB_PNG) $(LIB_CGAL) $(LIB) -o offline

offline.o: colormap.hpp export.hpp nns_base.hpp parser.hpp simulation.hpp types.hpp offline.cpp
	g++ $(OPT) $(OPT_PNG) $(OPT_CGAL) -c offline.cpp

simple: nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o simple.o simulation.o
	g++ nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o simple.o simulation.o $(LIB) -o simple

simple.o: nns_base.hpp simulation.hpp types.hpp simple.cpp
	g++ $(OPT) -c simple.cpp

# Modules

colormap.o: colormap.hpp colormap.cpp
	g++ $(OPT) -c colormap.cpp

export.o: export.hpp export.cpp
	g++ $(OPT) $(OPT_PNG) $(OPT_CGAL) -c export.cpp

nns_kd_tree.o: nns_base.hpp types.hpp nns_kd_tree.cpp
	g++ $(OPT) -c nns_kd_tree.cpp

nns_spatial_sorting.o: nns_base.hpp types.hpp nns_spatial_sorting.cpp
	g++ $(OPT) -c nns_spatial_sorting.cpp

nns_square_grid.o: nns_base.hpp types.hpp nns_square_grid.cpp
	g++ $(OPT) -c nns_square_grid.cpp

parser.o: parser.hpp parser.cpp
	g++ $(OPT) -c parser.cpp

simulation.o: simulation.hpp simulation.cpp
	g++ $(OPT) -c simulation.cpp

clean:
	rm -f pattern offline simple *.o
