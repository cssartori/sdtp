#include <iostream>
#include "../common/instance.hpp"
#include "../util/precise_timer.hpp"
#include "../validator/validator.hpp"
#include "../util/strfmt.hpp"
#include "../io/instance_io.hpp"
#include "../io/input.hpp"
#include "../io/perf_values_io.hpp"
#include "../solver/sdtp_solver.hpp"
#include "../solver/sdtp_solver_factory.hpp"
#include "../solver/sdtp_solver_perf.hpp"
#include "../io/perf_values.hpp"
#include <boost/program_options.hpp>
#include <random>
#include <regex>

std::vector< std::string > solver_names;
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

    std::vector< PerfValues > values;
    sdtp::SolverFactory sfact(instance);
    if(par.verbose) printf("Solving...\n");

    for(const std::string& sname : solver_names){
        if(par.verbose) printf("Building solver %s... ", sname.c_str());
        sdtp::Solver* solver = sfact.create(sname);
        sdtp::SolverPerf scperf(*solver, par.time_limit);
        if(par.verbose) printf("Measuring... ");
        scperf.run();
        values.push_back(scperf.get_values());
        if(par.verbose) printf("DONE\n");
        delete solver;
    }

    if(par.logger_path.size() > 0){
        sdtp::PerfValuesIo pvio(par.logger_path);
        bool w = pvio.write(instance, values);
        if(not w){
            printf("Error writing log file %s", par.logger_path.c_str());
        }
    }

    if(par.verbose){
        for(PerfValues val : values){
            val.print();
        }
    }

    return 0;
}

sdtp::Input load_cli(int argc, char** argv){
    sdtp::Input par;
    std::string str_snames = "{RULT,MDBF,CRA}";

	namespace po = boost::program_options;
	po::options_description desc("Parameters");
	desc.add_options()
		("help,h", "print this help info")
		("instance,i", po::value<std::string>(&par.instance_path)->required(),
		 ("The path to the input instance file to solve. This is a required argument."))
        ("algorithms,a", po::value<std::string>(&str_snames),
         ("The choice of algorithms and order to solve. Default: {RULT,MDBF,CRA}."))
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

	return par;
}
