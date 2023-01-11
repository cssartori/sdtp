#ifndef H_SDTP_INSTANCE_H
#define H_SDTP_INSTANCE_H

#include "dist_graph.hpp"
#include "domain.hpp"
#include <vector>
#include <assert.h>

namespace sdtp{
    class Instance{
    public:
        std::string name;
        DistGraph dgraph; //in dgraph a constraint Y - X <= w_xy is an arc (X,Y,w_xy)
        DistGraph rgraph; //in rgraph a constraint Y - X <= w_xy is an arc (Y,X,w_xy)
        size_t num_cons = 0;
        bool feasible = false;
        std::vector< std::vector<Domain> > var_domains;
        inline const static int node_zero = 0;

        Instance( ) {
            var_domains.push_back(std::vector<Domain>());
            var_domains[0].push_back({0,0}); //variable zero is always available to represent time t=0
        }

        int declare_variable(std::vector<Domain> doms){
            int n_d = rgraph.declare_node();
            int n_r = dgraph.declare_node();

            if(n_d != n_r){
                printf("ERROR!\n");
                exit(-1);
                return def::NO_NODE; //error
            }

            int n = n_d;
            if(doms.size() > 0){
                int lb = doms.front().lb;
                bool val_a = declare_constraint(node_zero, n, -lb);
                if(not val_a) return def::NO_NODE;
                var_domains.push_back(doms);
            }

            return n;
        }

        void add_max_bound_constraints(){
            for(size_t i=1;i<num_variables();i++){
                declare_constraint(i, node_zero, var_domains[i].back().ub);
            }
        }

        bool declare_constraint(int v1, int v2, int ub){ //v1 - v2 <= ub
            bool va = dgraph.declare_arc(v2, v1, ub);
            va = va && rgraph.declare_arc(v1, v2, ub);
            if(v1 != node_zero && v2 != node_zero) num_cons++;
            return va;
        }

        bool has_constraint(int v1, int v2) const{
            bool h = dgraph.has_arc(v2, v1) or rgraph.has_arc(v1, v2);
            return h;
        }

        int get_constraint_bound(int v1, int v2) const{
            return dgraph.get_arc(v2, v1).weight;
        }

        void update_constraint_bound(int v1, int v2, int wn){
            dgraph.update_arc_weight(v2, v1, wn);
            rgraph.update_arc_weight(v1, v2, wn);
        }

        size_t num_variables() const{
            return dgraph.num_nodes();
        }

        size_t num_constraints() const{
            return num_cons;
        }

        size_t variable_domain_size(int i) const{
            return var_domains[i].size();
        }

        size_t num_vars_multi_domains() const{
            size_t k = 0;
            for(size_t i=0;i<num_variables();i++){
                if(var_domains[i].size() > 1) k++;
            }
            return k;
        }

        size_t sum_multi_domains() const{
            size_t k = 0;
            for(size_t i=0;i<num_variables();i++){
                if(var_domains[i].size() > 1) k += var_domains[i].size();
            }
            return k;
        }

        size_t num_domains() const{
            size_t nd = 0;
            for(size_t i=0;i<num_variables();i++){
                nd += var_domains[i].size();
            }
            return nd;
        }

        size_t max_multi_dom() const{
            size_t k = 0;
            for(size_t i=0;i<num_variables();i++){
                k = std::max(var_domains[i].size(), k);
            }
            return k;
        }

        void set_feasibility(bool f){
            feasible = f;
        }

        bool is_feasible() const{
            return feasible;
        }

    };
}
#endif // H_SDTP_INSTANCE_H
