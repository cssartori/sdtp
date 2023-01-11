#ifndef H_SDTP_ULT_MODEL_H
#define H_SDTP_ULT_MODEL_H

#include "../../common/instance.hpp"
#include "../../common/dist_graph.hpp"
#include "../../common/domain.hpp"

namespace sdtp::ult{

    class UltModel{
    public:
        const DistGraph& graph;
        std::vector< std::vector< std::vector<Domain> > > con_domains; //constraint domains
        std::vector< std::pair<int,int> > con_list;
        const Instance& instance;

        UltModel(const Instance& instance_) : graph(instance_.dgraph), instance(instance_){
            con_domains.assign(graph.num_nodes(), std::vector< std::vector<Domain> >(graph.num_nodes(), std::vector<Domain>()));
            set_con_domains(instance);
        }

        void set_con_domains(const Instance& instance){
            const DistGraph& g = instance.dgraph;

            //construct con domains for i,j > 0
            for(int i=1;i<static_cast<int>(instance.num_variables());i++){
                for(int j=i+1;j < static_cast<int>(instance.num_variables());j++){
                    int min_ij = -def::MAX_INT;
                    int max_ij = def::MAX_INT;
                    int count = 0;

                    if(g.has_arc(i, j)){
                        max_ij = g.get_arc(i, j).weight;
                        count++;
                    }

                    if(g.has_arc(j, i)){
                        min_ij = -g.get_arc(j, i).weight;
                        count++;
                    }

                    if(count > 0){
                        con_domains[i][j].push_back(Domain(min_ij, max_ij));
                        con_list.push_back({i,j});
                    }
                }
            }

            //now construct con domains for the variable domains (0,i) // (i,0) arcs
            for(int i=1;i<static_cast<int>(instance.num_variables());i++){
                con_domains[instance.node_zero][i] = instance.var_domains[i];
                con_list.push_back({0,i});
            }
        }

        bool has_domain_changed(const std::vector<Domain>& dv1, const std::vector<Domain>& dv2){
            if(dv1.size() != dv2.size()) return true;
            bool diff = false;
            for(size_t k=0;k<dv1.size();k++){
                if(dv1[k] != dv2[k]){
                    diff = true;
                    break;
                }
            }
            return diff;
        }

        bool intersect_con_domain(int i, int j, Domain dom){
            std::vector<Domain> vd;
            for(const Domain& dij : con_domains[i][j]){
                bool inter;
                Domain in_d;
                std::tie(inter, in_d) = dom.intersection(dij);
                if(inter) vd.push_back(in_d);
            }

            bool change = false;
            if(has_domain_changed(con_domains[i][j], vd)){
                con_domains[i][j] = vd;
                if(vd.size() == 0) return true;
                change = true;
            }
            return change;
        }

        void reset(){
            con_list.clear();
            con_domains.assign(graph.num_nodes(), std::vector< std::vector<Domain> >(graph.num_nodes(), std::vector<Domain>()));
            set_con_domains(instance);
        }

    };
}
#endif //H_SDTP_ULT_MODEL_H
