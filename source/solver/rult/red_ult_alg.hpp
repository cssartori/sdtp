#ifndef H_SDTP_RULT_ALG_H
#define H_SDTP_RULT_ALG_H

#include "../solver.hpp"
#include "../../common/instance.hpp"
#include "two_ways_bf_alg.hpp"
#include "../../util/precise_timer.hpp"

namespace sdtp::rult{

    class RedUltAlg : public Solver{
    public:
        RedUltAlg(Instance instance_);
        const Instance& get_instance() const;
        std::string get_name() const;
        void extract_solution(std::vector<int>& sol) const;
        long get_runtime_microseconds() const;
        long get_runtime_milliseconds() const;
        long get_num_scans() const;
        bool timedout() const;
        bool run(long time_limit=def::MAX_LONG);
        void reset();

        void get_bounds(std::vector<int>& low, std::vector<int>& upp);

    private:
        std::string name;
        Instance instance;
        std::vector< std::vector<Domain> > var_domains;
        std::vector< std::pair<int, int> > di; //domain indices
        std::vector< int > fd, bd;
        TwoWaysBfAlg twbf;
        util::PreciseTimer timer;
        long scans;
        bool timeout;

        bool compute_bounds();
        bool has_domain_changed(const std::vector<Domain>& dv1, const std::vector<Domain>& dv2);
        bool intersect_var_domains(size_t i, Domain dom);
        int tighten_bounds();
    };
}
#endif //H_SDTP_RULT_ALG_H
