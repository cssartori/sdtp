#include "sdtp_solver_exec.hpp"

namespace sdtp{
Result SolverExec::run(int num_rep, bool verbose){
    result.reset();
    Validator val(solver.get_instance());
    int r = 0;

    while(r < num_rep){
        sol.clear();
        bool feas = solver.run(time_limit);
        bool vsol = false;
        if(feas){
            solver.extract_solution(sol);
            vsol = val.validate(sol, verbose);
        }
        last_sol_feas = vsol;

        std::string msg = "ok";
        bool error = false;
        if(feas && not vsol){
            error = true;
            msg = util::str::format("In repetition %d result is feasible but solution is invalid.", r+1);
        }

        bool timedout = solver.timedout();
        if(not error){
            if(feas != solver.get_instance().is_feasible() && not timedout){
                error = true;
                msg = util::str::format("In repetition %d result differs from instance feasibility : %s != %s", r+1, feas ? "feasible" : "infeasible", solver.get_instance().is_feasible() ? "feasible" : "infeasible");
            }
        }

        result.append_result(feas, vsol, solver.get_runtime_microseconds(), timedout, error, solver.get_num_scans()*1.0/solver.get_instance().num_variables(), msg);
        solver.reset();
        r++;
    }

    return result;
}

void SolverExec::extract_last_solution(std::vector<int>& sol_){
    if(last_sol_feas) sol_ = sol;
}

}
