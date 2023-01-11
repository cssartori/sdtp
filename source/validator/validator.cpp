#include "validator.hpp"

namespace sdtp{
    bool Validator::is_domain_valid(int i, int v) const{
        for(const Domain& dom : instance.var_domains[i]){
            if(dom.contains(v)){
                return true;
            }
        }
        return false;
    }

    bool Validator::validate(const std::vector<int>& sol, bool print) const{
        bool feas = true;
        for(size_t i=0;i<instance.num_variables();i++){
            if(not is_domain_valid(i, sol[i])){
                if(print){
                    printf("Solution of %ld : %d is not within domains ", i, sol[i]);
                    for(const Domain& dom : instance.var_domains[i]){
                        printf("[%d,%d] ", dom.lb, dom.ub);
                    }
                    printf("\n");
                }
                feas = false;
            }
        }

        if(not feas) return false;
        for(size_t i=1;i<instance.num_variables();i++){
            for(const Arc& a : instance.rgraph.arcs[i]){
                int j = a.target;
                int w = a.weight;
                if(sol[i] - sol[j] > w){
                    if(print) printf("Constraint %ld - %d <= %d is invalid : %d - %d > %d\n", i, j, w, sol[i], sol[j], w);
                    feas = false;
                }
            }
        }

        return feas;
    }
}
