#ifndef H_SDTP_ILP_H
#define H_SDTP_ILP_H

#include "../sdtp_solver.hpp"
#include "../../common/instance.hpp"
#include "../../util/strfmt.hpp"
#include "../../util/precise_timer.hpp"

//the lines below supress CPLEX-related warnings during compilation
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <ilcplex/ilocplex.h>
#pragma GCC diagnostic pop

//Integer Linear Programming model to solve SDTPs using CPLEX

namespace sdtp::ilp{
    class Ilp : public Solver{
    public:
        Ilp(const Instance& instance_);
        ~Ilp();
        const Instance& get_instance() const;
        std::string get_name() const;
        long get_runtime_microseconds() const;
        long get_runtime_milliseconds() const;
        long get_num_scans() const{ return 0; }
        bool timedout() const;
        bool run(long time_limit=def::MAX_LONG);
        void extract_solution(std::vector<int>& sol) const;
        void reset();

    private:
        std::string name;
        IloEnv env;
        IloModel *model;
        IloCplex *cplex;
        const Instance& instance;
        IloNumVarArray h;
        std::vector<IloIntVarArray> y;
        const bool SET_NAMES = false;
        util::PreciseTimer timer;
        bool timeout;

        void build_model();
    };
}

#endif //H_SDTP_ILP_H
