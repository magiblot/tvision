CXX = g++ -m32
CXXFLAGS = -O0 -g -gstabs+ --std=c++98 -fpermissive -D __FLAT__ -D __cdecl= -D __stdcall=
INCLUDES = -Iinclude -Iinclude/windows
SOURCES = source/tvision/*.cpp
OUTDIR = lib/

all:
	$(CXX) -c $(SOURCES) $(INCLUDES) $(CXXFLAGS)
