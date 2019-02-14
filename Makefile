#compiler to be used
CXX = g++

#compilation flags to be used
CXXFLAGS = -Wall -O3 -std=c++14

#location of files and libraries to be used
OSRM_DIR = ../osrm-backend/
#GDAL_DIR = ../gdal-2.3.0/

SRCDIR = src/
INCDIR = include/

INCLUDES = -I$(OSRM_DIR)include/ -I$(OSRM_DIR)third_party/variant/include/ #-I$(GDAL_DIR)
LIBS = -losrm -lm -lpthread -lrt -lboost_filesystem -lboost_iostreams -lboost_program_options -lboost_system -lboost_thread #-lgdal
LIBDIR = -L$(OSRM_DIR)lib -L/usr/local/lib

# define the C++ source files
SMAIN = $(SRCDIR)main.cpp $(SRCDIR)distributer.cpp $(SRCDIR)router.cpp $(SRCDIR)instance_creator.cpp $(SRCDIR)configurations.cpp $(SRCDIR)geolocation.cpp $(SRCDIR)builder.cpp $(SRCDIR)pdptw_builder.cpp $(SRCDIR)cvrp_builder.cpp $(SRCDIR)instance.cpp
OMAIN = $(SMAIN:.cpp=.o)

# define the executable file 
MAIN = main

.PHONY: depend clean

all: $(MAIN)
	@echo  main compiled

$(MAIN): $(OMAIN)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBDIR) -o $(MAIN) $(OMAIN) $(LIBS)

.cpp.o: 
	$(CXX) $(CXXFLAGS) $(DDFS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(SRCDIR)*.o
	$(RM) $(SRCDIR)*.bak 

depend: $(SMAIN)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
