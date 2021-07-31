#include <vector>
#include <cmath>
#include <random>
#include <limits>

#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <cstdlib>
#include <boost/program_options.hpp>
#include "../include/common.hpp"
#include "../include/configurations.hpp"
#include "../include/instance.hpp"
#include "../include/instance_creator.hpp"

// declare the random number generator
std::mt19937_64 rng;
std::string configuration_file = DEFAULT_CONFIG_FILE;

void read_parameters(int argc, char** argv, Configurations& con);

int main(int argc, char** argv){
	Configurations con;
	read_parameters(argc, argv, con);
	//printf("Seed: %ld\n", con.seed);
	rng = std::mt19937_64(con.seed);

	Instance inst;
	if(con.verbose >= 1) printf("Creating instance: getting locations, routing paths, and generating remaining data...\n");
	int ret = InstanceCreator::create(con, inst);	
	if(ret == EXIT_FAILURE) return ret;
	else if(con.verbose >= 1) printf("DONE: Instance created.\n");
	
	if(con.verbose >= 1) printf("Writing instance to file %s...\n", con.inst_file.c_str());
	FileCode rwrite = Instance::write_instance_file(con.inst_file, inst);
	ret = rwrite == FileCode::WriteError ? EXIT_FAILURE : EXIT_SUCCESS;
	if(ret != EXIT_FAILURE && con.verbose >= 1) printf("DONE: Instance written to file.\n");

	return ret;
}

void read_parameters(int argc, char** argv, Configurations& con){
	size_t temp_seed = std::numeric_limits<size_t>::max();
	con.seed = 0;
	con.verbose = 1;
	con.inst_file = DEFAULT_INST_FILE;
	
	namespace po = boost::program_options;
	po::options_description desc("parameters");
	desc.add_options()
		("help,h", "print this help info")
		("configuration,c", po::value<std::string>(&configuration_file)->required(),
		 ("Configuration file to read the parameters."))
		("output,o", po::value<std::string>(&con.inst_file),
		 ("output file where the generated instance will be recorded. Default: " + DEFAULT_INST_FILE).c_str())
		("seed,s", po::value<size_t>(&temp_seed),
		 ("define the RNG seed. Default: 0"))
		("verbose,v", po::value<int>(&con.verbose),
		 ("verbosity level to print intermediate information. Values: 0 (none), 1 (some), 2 (full). Default: 1"))
		; 

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(EXIT_SUCCESS);
	}

	po::notify(vm);
	
	if(con.verbose >= 1) printf("Opening configuration file %s...\n", configuration_file.c_str());
	std::ifstream cfile(configuration_file);
	if(not cfile.is_open()){
		printf("Could not open file \"%s\"\n", configuration_file.c_str());
		exit(EXIT_FAILURE);
	}
	
	if(con.verbose >= 1){
		printf("DONE: configuration file opened.\n");
		printf("Reading configuration...\n");
	}
	if(Configurations::read_configurations(con, cfile) == FileCode::ReadError){
		printf("Error reading configuration from file %s\n", configuration_file.c_str());
		cfile.close();
		exit(EXIT_FAILURE);
	}
	if(con.verbose >= 1) printf("DONE: Configuration read.\n");
	
	cfile.close();
	if(temp_seed != std::numeric_limits<size_t>::max()){
		con.seed = temp_seed;
	}
}
