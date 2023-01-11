#ifndef H_SDTP_RESULT_H
#define H_SDTP_RESULT_H

#include <string>
#include <vector>
#include "../common/def.hpp"

namespace sdtp{
    struct Result{
        std::string name;
        std::vector<bool> feas_flags;
        std::vector<bool> sol_flags;
        std::vector<long> runtimes;
        std::vector<bool> timeouts;
        std::vector<bool> errors;
        bool error = false;
        std::vector<double> avg_scans;
        std::vector< std::string > msgs;

        Result(const std::string& name_) : name(name_){ }

        void reset(){
            feas_flags.clear();
            sol_flags.clear();
            runtimes.clear();
            errors.clear();
            timeouts.clear();
            error = false;
            avg_scans.clear();
            msgs.clear();
        }

        void print_summary() const{
            printf("%s :: %s :: %s solution :: %ld :: %.2f :: %s -> %s\n", name.c_str(), is_feasible() ? "feasible" : "infeasible", is_solution_valid() ? "good" : "bad", average_runtime(), average_scans(), timedout() ? "timeout" : "ok", error ? "ERROR" : "OK");
        }

        void print_all() const{
            printf("Results for %s: %lu entries\n", name.c_str(), feas_flags.size());
            for(size_t i=0;i<feas_flags.size();i++){
                printf("\tEntry %lu: %s | %s | %ld | %s | %s | %.2f | %s\n", i, feas_flags[i] ? "feasible" : "infeasible", sol_flags[i] ? "valid" : "invalid", runtimes[i], timeouts[i] ? "timeout" : "ok", errors[i] ? "error" : "ok", avg_scans[i], msgs[i].c_str());
            }
        }

        void append_result(bool feas, bool sol, long runtime, bool timeout, bool err, double scans, const std::string& msg){
            feas_flags.push_back(feas);
            sol_flags.push_back(sol);
            runtimes.push_back(runtime);
            timeouts.push_back(timeout);
            errors.push_back(err);
            avg_scans.push_back(scans);
            msgs.push_back(msg);
            error = error || err;
        }

        bool is_feasible() const{
            bool feas = true;
            for(bool b : feas_flags){
                feas = feas && b;
            }
            return feas;
        }

        bool is_solution_valid() const{
            bool val = true;
            for(bool b : sol_flags){
                val = val && b;
            }
            return val;
        }

        bool timedout() const{
            bool timed = false;
            for(bool b : timeouts){
                timed = timed || b;
            }
            return timed;
        }

        long average_runtime() const{
            long sum = 0;
            for(long t : runtimes){
                sum += t;
            }
            return (sum/runtimes.size());
        }

        double average_scans() const{
            double sum = 0;
            for(double s : avg_scans){
                sum += s;
            }
            return (sum/avg_scans.size());
        }


        bool has_error() const{
            return error;
        }
    };
}

#endif //H_SDTP_RESULT_H
