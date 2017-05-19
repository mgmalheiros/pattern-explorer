OPTIONS = -O2 -Wall -Wextra -I/home/mgm/local/packages/include# -g
LIBS	= -L/home/mgm/local/packages/lib -lrt

ATB     = -lAntTweakBar
CGAL    = -lCGAL -lboost_system -lgmp
OPENGL	= -lglut -lGL
PNG		= -lpng

all: pattern offline simple

# PROGRAMS

pattern: colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o pattern.o simulation.o
	g++  colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o pattern.o simulation.o $(LIBS) $(ATB) $(CGAL) $(OPENGL) $(PNG) -o pattern 

pattern.o: colormap.hpp export.hpp nns_base.hpp parser.hpp simulation.hpp types.hpp pattern.cpp
	g++ $(OPTIONS) -c pattern.cpp

offline: colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o offline.o simulation.o
	g++  colormap.o export.o nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o parser.o offline.o simulation.o $(LIBS) $(CGAL) $(PNG) -o offline

offline.o: colormap.hpp export.hpp nns_base.hpp parser.hpp simulation.hpp types.hpp offline.cpp
	g++ $(OPTIONS) -c offline.cpp

simple: nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o simple.o simulation.o
	g++ nns_kd_tree.o nns_spatial_sorting.o nns_square_grid.o simple.o simulation.o $(LIBS) -o simple 

simple.o: nns_base.hpp simulation.hpp types.hpp simple.cpp
	g++ $(OPTIONS) -c simple.cpp

# MODULES

colormap.o: colormap.hpp colormap.cpp
	g++ $(OPTIONS) -c colormap.cpp 

export.o: export.hpp export.cpp
	g++ $(OPTIONS) -c export.cpp 

nns_kd_tree.o: nns_base.hpp types.hpp nns_kd_tree.cpp
	g++ $(OPTIONS) -c nns_kd_tree.cpp 

nns_spatial_sorting.o: nns_base.hpp types.hpp nns_spatial_sorting.cpp
	g++ $(OPTIONS) -c nns_spatial_sorting.cpp 

nns_square_grid.o: nns_base.hpp types.hpp nns_square_grid.cpp
	g++ $(OPTIONS) -c nns_square_grid.cpp 

parser.o: parser.hpp parser.cpp
	g++ $(OPTIONS) -c parser.cpp 

simulation.o: simulation.hpp simulation.cpp
	g++ $(OPTIONS) -c simulation.cpp 

clean:
	rm -f pattern offline simple *.o
