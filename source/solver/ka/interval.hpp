#ifndef H_SDTP_KA_INTERVAL_H
#define H_SDTP_KA_INTERVAL_H

#include "../../common/domain.hpp"

namespace sdtp::ka{
    struct Interval{
        int v;
        int d;
        int lb;
        int ub;
        int pref;

        Interval(int v_, int d_, int lb_, int ub_, int p){
            v = v_;
            d = d_;
            lb = lb_;
            ub = ub_;
            pref = p;
        }

        Interval(int v_, int d_, const Domain& dom, int p){
            v = v_;
            d = d_;
            lb = dom.lb;
            ub = dom.ub;
            pref = p;
        }
    };
}

#endif //H_SDTP_KA_INTERVAL_H
