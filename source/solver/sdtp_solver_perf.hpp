#ifndef H_SDTP_SOLVER_PERF_H
#define H_SDTP_SOLVER_PERF_H

#include "../validator/validator.hpp"
#include "../util/strfmt.hpp"
#include "sdtp_solver.hpp"
#include "../io/perf_values.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

namespace sdtp{

    class SolverPerf{
    public:
        SolverPerf(Solver& solver_, long time_limit_=def::MAX_LONG) : solver(solver_), time_limit(time_limit_), values(solver.get_name()) { }

        void run();
        PerfValue get_value(int conf) const;
        PerfValues get_values() const;

    private:
        Solver& solver;
        long time_limit;
        bool feas;

        struct read_format {
            uint64_t nr;
            struct {
                uint64_t value;
                uint64_t id;
            } values[];
        };

        std::vector<long> fds;
        std::vector<long> leaders;
        std::vector< std::vector<uint64_t> > ids;
        PerfValues values;
        std::vector<struct perf_event_attr> peas;

        struct perf_event_attr create_pea(uint64_t type, uint64_t conf);
        long create_event(uint64_t type, uint64_t conf, long leader);
        void setup_measurements();
        void read_values();
        void close_all();
    };
}

#endif //H_SDTP_SOLVER_PERF_H
