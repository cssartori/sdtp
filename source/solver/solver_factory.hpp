#ifndef H_SDTP_SOLVER_FACTORY_H
#define H_SDTP_SOLVER_FACTORY_H

#include "solver.hpp"
#include <random>

namespace sdtp{
    class SolverFactory{
    private:
        const Instance& instance;

    public:
        SolverFactory(const Instance& instance_) : instance(instance_) {

        }

        Solver* create(std::string solver_name) const;
        static bool solver_exists(std::string solver_name);

    };
}

#endif //H_SDTP_SOLVER_FACTORY_H
