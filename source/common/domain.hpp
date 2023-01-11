#ifndef H_STN_DOMAIN_H
#define H_STN_DOMAIN_H

#include "def.hpp"
#include <vector>

namespace sdtp{
    struct Domain{
        int lb = def::MIN_INT;
        int ub = def::MAX_INT;

        Domain(int l=def::MIN_INT, int u=def::MAX_INT){
            lb = l;
            ub = u;
        }

        bool contains(int value) const{
            if(lb <= value && value <= ub) return true;
            return false;
        }

        std::pair<bool, Domain> intersection(const Domain& dom) const{
            int max_lb = std::max(dom.lb, this->lb);
            int min_ub = std::min(dom.ub, this->ub);
            bool has_intersec = (min_ub >= max_lb);
            return std::pair<bool, Domain>(has_intersec, Domain(max_lb, min_ub));
        }

        bool operator==(const Domain& other) const{
            return (this->lb == other.lb && this->ub == other.ub);
        }

        bool operator!=(const Domain& other) const{
            return !(*this == other);
        }

        int size() const{
            return ub - lb;
        }
    };
}

#endif //H_STN_DOMAIN_H
