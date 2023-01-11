#include <iostream>
#include "../common/instance.hpp"
#include "../util/precise_timer.hpp"
#include "../validator/validator.hpp"
#include "../util/strfmt.hpp"
#include "../io/instance_io.hpp"
#include "../io/input.hpp"
#include "../io/result_io.hpp"
#include "../solver/solver.hpp"
#include "../solver/solver_factory.hpp"
#include "../solver/solver_exec.hpp"
#include "../io/result.hpp"
#include <boost/program_options.hpp>
#include <random>
#include <regex>

std::string algorithm;

sdtp::Input load_cli(int argc, char** argv);

int main(int argc, char *argv[]) {
    sdtp::Input par = load_cli(argc, argv);

    sdtp::Instance instance;
    sdtp::InstanceIo iio(par.instance_path);
    if(par.verbose) printf("Reading instance file...\n");
    if(not iio.read(instance)){
        printf("Failed to read instance file.\n");
        printf("%s\n", iio.get_message().c_str());
        exit(EXIT_FAILURE);
    }

    sdtp::Validator val(instance);

    if(par.verbose) printf("Building solver...\n");

    sdtp::SolverFactory sfact(instance);
    sdtp::Solver* solver = sfact.create(algorithm);
    sdtp::SolverExec sexec(*solver, par.time_limit);

    if(par.verbose) printf("Solving...\n");
    sdtp::Result result = sexec.run(1, par.verbose);

    std::vector<int> sol;
    sexec.extract_last_solution(sol);

    if(par.verbose) result.print_summary();

    if(instance.is_feasible() && (not result.is_feasible() or not result.is_solution_valid())){
        printf("*Warning: possible error in solution. Instance is feasible but ");
        if(not result.is_feasible()) printf(" %s states it is infeasible.\n", algorithm.c_str());
        else printf(" %s provided an invalid solution.\n", algorithm.c_str());
    }

    if(par.logger_path.size() > 0){
        sdtp::ResultIo rio(par.logger_path);
        bool status = rio.write(instance, result, sol);
        if(not status){
            printf("Error writing log file %s", par.logger_path.c_str());
        }
    }

    delete solver;
    return 0;
}

sdtp::Input load_cli(int argc, char** argv){
    sdtp::Input par;

	namespace po = boost::program_options;
	po::options_description desc("Parameters");
	desc.add_options()
		("help,h", "print this help info")
        ("algorithm,a", po::value<std::string>(&algorithm)->required(),
		 ("The name of the algorithm to use when solving the input instance. One of: BFDC, RULT, ILP, CP, SCP, KAB, KAJ, CRA, ULT."))
		("instance,i", po::value<std::string>(&par.instance_path)->required(),
		 ("The path to the input instance file to solve. This is a required argument."))
		("log", po::value<std::string>(&par.logger_path),
		 ("Path to a text file to where the result of the execution will be written (including detailed solution)."))
        ("verbose", po::bool_switch(&par.verbose),
         ("Whether to print execution messages and summarized results to stdout."))
        ("time-limit", po::value<long>(&par.time_limit),
         ("Time limit in microseconds. Default: "+std::to_string(sdtp::def::TIME_LIMIT)).c_str())
		;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(EXIT_SUCCESS);
	}
    po::notify(vm); // seems to be what put the read values in the variables

    if(not sdtp::SolverFactory::solver_exists(algorithm)){
        printf("Unkown algorithm option '%s'\n", algorithm.c_str());
        exit(EXIT_FAILURE);
    }

    if(par.time_limit < 1){
        printf("Invalid time limit.\n");
        exit(EXIT_FAILURE);
    }

	return par;
}
