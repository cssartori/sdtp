#ifndef H_SDTP_SOLVER_H
#define H_SDTP_SOLVER_H

//base, abstract class for a SDTP solver

#include <vector>
#include <string>
#include <limits>
#include "../common/instance.hpp"

namespace sdtp{
    class Solver{
    public:
        virtual ~Solver() { };
        virtual bool run(long time_limit) = 0;
        virtual void extract_solution(std::vector<int>& sol) const = 0;
        virtual long get_runtime_microseconds() const = 0;
        virtual long get_runtime_milliseconds() const = 0;
        virtual long get_num_scans() const = 0;
        virtual bool timedout() const = 0;
        virtual std::string get_name() const = 0;
        virtual const Instance& get_instance() const = 0;
        virtual void reset() = 0;
    };
}

#endif //H_SDTP_SOLVER_H
