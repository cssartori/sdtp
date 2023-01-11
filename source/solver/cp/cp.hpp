#ifndef H_SDTP_CP_H
#define H_SDTP_CP_H

#include "../sdtp_solver.hpp"
#include "../../util/precise_timer.hpp"
#include <ilcp/cp.h>
#include <map>
#include <vector>

//the lines below supress CPLEX-related warnings during compilation
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <ilcplex/ilocplex.h>
#pragma GCC diagnostic pop

//Constraint Programming model to solve SDTPs using CPLEX

namespace sdtp::cp{
    class Cp : public Solver{
    public:
        Cp(const Instance& instance_, bool simp_);
        ~Cp();
        const Instance& get_instance() const;
        std::string get_name() const;
        long get_runtime_microseconds() const;
        long get_runtime_milliseconds() const;
        long get_num_scans() const;
        bool timedout() const;
        bool run(long time_limit=def::MAX_LONG);
        void extract_solution(std::vector<int>& sol) const;
        void reset();


    private:
        std::string name;
        IloEnv env;
        IloModel* model;
        IloCP *cp;
        const Instance& instance;
        IloIntVarArray h;
        util::PreciseTimer timer;
        bool simp;
        bool timeout;

        void build_model();

    };
}

#endif //H_SDTP_CP_H
