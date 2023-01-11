#ifndef H_SDTP_VALIDATOR_H
#define H_SDTP_VALIDATOR_H

#include "../common/instance.hpp"
#include <vector>

namespace sdtp{
    class Validator{
    private:
        const Instance& instance;
        bool is_domain_valid(int i, int v) const;

    public:
        Validator(const Instance& instance_) : instance(instance_) { }
        bool validate(const std::vector<int>& sol, bool print=false) const;
    };
}
#endif //H_SDTP_VALIDATOR_H
