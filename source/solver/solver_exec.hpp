#ifndef H_SDTP_SOLVER_EXEC_H
#define H_SDTP_SOLVER_EXEC_H

#include "../io/result.hpp"
#include "../validator/validator.hpp"
#include "../util/strfmt.hpp"
#include "solver.hpp"
#include "../common/def.hpp"

namespace sdtp{
    class SolverExec{
    private:
        Solver& solver;
        Result result;
        long time_limit;
        bool last_sol_feas;
        std::vector<int> sol;

    public:
        SolverExec(Solver& solver_, long time_limit_=def::MAX_LONG) : solver(solver_), result(solver.get_name()), time_limit(time_limit_) { }
        Result run(int num_rep, bool verbose=false);
        void extract_last_solution(std::vector<int>& sol_);

    };
}
#endif //H_SDTP_SOLVER_EXEC_H
