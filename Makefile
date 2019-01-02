CXX = g++ -m32
CXXFLAGS = -O0 -g -gstabs+ --std=c++98 -fpermissive -D __FLAT__
INCLUDES = -Iinclude
SOURCES = source/tvision/*.cpp
OUTDIR = lib/

all:
	$(CXX) -c $(SOURCES) $(INCLUDES) $(CXXFLAGS)
