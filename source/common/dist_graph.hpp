#ifndef H_SDTP_DIST_GRAPH_H
#define H_SDTP_DIST_GRAPH_H

//Distance graph associated with a Simple Temporal Network

#include <vector>
#include <map>
#include "arc.hpp"

namespace sdtp{
    struct DistGraph{
        std::vector< std::vector<Arc> > arcs;
        std::map< std::pair<int,int> , size_t > arc_src_index; // (arc(i,j,w_ij)) -> index in arcs[i]
        size_t arc_count = 0;

        DistGraph(size_t num_nodes=1){
            if(num_nodes > 1)
                arcs.reserve(num_nodes);

            arcs.push_back(std::vector<Arc>()); //node zero is always available
        }

        int declare_node(){
            arcs.push_back(std::vector<Arc>());
            return static_cast<int>(arcs.size()-1);
        }

        bool declare_arc(int source, int target, int weight){
            if(not has_node(source) || not has_node(target)) return false;

            arcs[source].push_back(Arc(source, target, weight));
            arc_src_index[std::make_pair(source, target)] = arcs[source].size()-1;
            arc_count++;

            return true;
        }

        bool update_arc_weight(int source, int target, int new_weight){
            size_t aix = arc_src_index[{source,target}];
            arcs[source][aix].weight = new_weight;
            return true;
        }

        int node_zero() const{
            return 0;
        }

        bool has_node(int n) const{
            if(n < 0 || n >= static_cast<int>(arcs.size())) return false;
            return true;
        }

        bool has_arc(int source, int target) const{
            if(arc_src_index.count({source,target}) == 0) return false;
            return true;
        }

        Arc get_arc(int source, int target) const{
            return arcs[source][arc_src_index.at({source,target})];
        }

        size_t num_nodes() const{
            return arcs.size();
        }

        size_t num_arcs() const{
            return arc_count;
        }

        int get_longest_arc_weight() const{
            int w = 0;
            for(const auto& ap : arc_src_index){
                w = std::max(w, arcs[ap.first.first][ap.second].weight);
            }
            return w;
        }

        void remove_arc(int source, int target){
            int k = 0;
            for(Arc a : arcs[source]){
                if(a.target == target){
                    break;
                }
                k++;
            }
            arcs[source].erase(std::next(arcs[source].begin(), k));
            arc_src_index.erase(std::pair<int,int>(source,target));
        }

    };
}

#endif //H_SDTP_DIST_GRAPH_H
