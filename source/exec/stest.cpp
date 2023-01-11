#include <iostream>
#include "../common/instance.hpp"
#include "../util/precise_timer.hpp"
#include "../validator/validator.hpp"
#include "../util/strfmt.hpp"
#include "../io/result_io.hpp"
#include "../io/instance_io.hpp"
#include "../io/input.hpp"
#include "../solver/solver.hpp"
#include "../solver/solver_exec.hpp"
#include "../solver/solver_factory.hpp"
#include "../io/result.hpp"
#include <boost/program_options.hpp>
#include <random>
#include <regex>
//#include <valgrind/callgrind.h>

std::vector< std::string > solver_names;
sdtp::Input load_cli(int argc, char** argv);

std::string check_for_error(bool feasibility, const std::vector<sdtp::Result>& results){
    bool feas = feasibility;
    bool vsol = false;
    for(const sdtp::Result& res : results){
        if(not res.timedout()){
            vsol = res.is_solution_valid();
            break;
        }
    }

    std::string msg = "ok";
    for(const sdtp::Result& res : results){
        if(res.timedout()) continue;
        if(res.has_error() || res.is_feasible() != feas || res.is_solution_valid() != vsol){
            msg = "error";
            break;
        }
    }
    return msg;
}

bool are_solutions_equal(std::vector<int>& s1, std::vector<int>& s2){
  for(size_t i=0;i<s1.size();i++){
    if(s1[i] != s2[i]){
      return false;
    }
  }
  return true;
}

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

    if(par.verbose) printf("Allocating basic structures...\n");
    std::vector< sdtp::Result > results;
    sdtp::SolverFactory sfact(instance);
    if(par.verbose) printf("Solving...\n");

    for(const std::string& sname : solver_names){
        if(par.verbose) printf("Building solver %s...\n", sname.c_str());
        sdtp::Solver* solver = sfact.create(sname);
        sdtp::SolverExec sexec(*solver, par.time_limit);
        if(par.verbose) printf("\twith %s\n", sname.c_str());
        sdtp::Result res = sexec.run(par.num_repetitions, par.verbose);
        results.push_back(res);
        results.back().print_summary();
        //results.back().print_all();
        delete solver;
    }

    std::string msg = check_for_error(instance.is_feasible(), results);
    if(msg == "error"){
        if(par.verbose) printf("ERROR occurred for %s\n", par.instance_path.c_str());
    }

    if(par.logger_path.size() > 0){
        sdtp::ResultIo rio(par.logger_path);
        bool status = rio.write(instance, results, msg);
        if(not status){
            printf("Error writing log file %s", par.logger_path.c_str());
        }
    }

    return 0;
}

sdtp::Input load_cli(int argc, char** argv){
    sdtp::Input par;
    std::string str_snames = "{ULT,RULT,ILP,CP,SCP,KAB,KAJ,BFDC,CRA}";

	namespace po = boost::program_options;
	po::options_description desc("Parameters");
	desc.add_options()
		("help,h", "print this help info")
		("instance,i", po::value<std::string>(&par.instance_path)->required(),
		 ("The path to the input instance file to solve. This is a required argument."))
        ("algorithms,a", po::value<std::string>(&str_snames),
         ("The choice of algorithms and order to solve. Default: {ULT,RULT,ILP,CP,SCP,KAB,KAJ,MDBF,CRA}."))
		("log", po::value<std::string>(&par.logger_path),
		 ("Path to a text file where the log of all tests will be written (this does not include a solution to the instance, for this please use 'stdp')."))
        ("verbose", po::bool_switch(&par.verbose),
         ("Whether to print execution information and summarized results to stdout."))
        ("time-limit", po::value<long>(&par.time_limit),
         ("Time limit in microseconds. Default: "+std::to_string(sdtp::def::TIME_LIMIT)).c_str())
        ("num-repetitions", po::value<int>(&par.num_repetitions),
         ("Number of times to run each algorithm. Default: "+std::to_string(par.num_repetitions)).c_str())
		;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(EXIT_SUCCESS);
	}

    po::notify(vm); // seems to be what put the read values in the variables


    std::regex rgx(R"([a-zA-Z]+)");
    std::sregex_iterator iter(str_snames.begin(), str_snames.end(), rgx);
    std::sregex_iterator end;
    while(iter != end) {
        for(unsigned i = 0; i < iter->size(); ++i) {
            if(sdtp::SolverFactory::solver_exists((*iter)[i])){
                solver_names.push_back((*iter)[i]);
            }else{
                printf("Unkown algorithm option %s\n", (*iter)[i].str().c_str());
                exit(EXIT_FAILURE);
            }

        }
        iter++;
    }

    if(par.time_limit < 1){
        printf("Invalid time limit.\n");
        exit(EXIT_FAILURE);
    }

    if(par.num_repetitions < 1){
        printf("Invalid number of repetitions.\n");
        exit(EXIT_FAILURE);
    }

	return par;
}
