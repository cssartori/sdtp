#ifndef H_SDTP_KA_MAX_FLOW_GRAPH_H
#define H_SDTP_KA_MAX_FLOW_GRAPH_H

#include <vector>
#include <tuple>
#include "interval.hpp"
#include "../../common/def.hpp"

namespace sdtp::ka{

    struct MfNode{
        int i; //interval index this node represents
        int w; //weight of this node
        int pair;
        bool prime;

        MfNode(int i_, int w_, int p_, bool isp){
            i = i_; w = w_; pair = p_;
            prime = isp;
        }

        bool is_prime() const{
            return prime;
        }
    };

    struct MfArc{
        int from;
        int to;
        int capacity;
        int flow;
        int revi; //reverse arc index

        MfArc(int f, int t, int c){
            from = f;
            to = t;
            capacity = c;
            flow = 0;
        }

        int residual_capacity() const{
            return capacity - flow;
        }

    };

    struct MaxFlowGraph{
        std::vector<MfNode> nodes;
        int num_orig_nodes;
        int source;
        int sink;
        std::vector< std::vector<MfArc> > arcs;

        void add_node(int i, int w){
            nodes.push_back(MfNode(i, w, -1, false));
            arcs.push_back(std::vector<MfArc>());
        }

        void add_arc(int from, int to, int cap){
            arcs[from].push_back(MfArc(from, to, cap));
            arcs[to].push_back(MfArc(to, from, 0)); //reverse

            arcs[from][arcs[from].size()-1].revi = (int)arcs[to].size()-1;
            arcs[to][arcs[to].size()-1].revi = (int)arcs[from].size()-1;
        }

        void duplicate_nodes(){
            num_orig_nodes = (int)nodes.size();
            for(int n=0;n<num_orig_nodes;n++){
                nodes.push_back(nodes[n]);
                nodes[n].pair = (int)nodes.size()-1;
                nodes.back().pair = n;
                nodes.back().prime = true;
                arcs.push_back(std::vector<MfArc>());
            }
        }

        void clear(){
            nodes.clear();
            num_orig_nodes = 0;
            source = -1;
            sink = -1;
            arcs.clear();
        }

        void add_source_and_sink(){
            source = (int)nodes.size();
            sink = source+1;
            nodes.push_back(MfNode(-1, 0, -1, false));
            nodes.push_back(MfNode(-1, 0, -1, false));

            arcs.push_back(std::vector<MfArc>());
            arcs.push_back(std::vector<MfArc>());

            for(int n=0;n<num_orig_nodes;n++){
                add_arc(source, n, nodes[n].w);
                add_arc(nodes[n].pair, sink, nodes[n].w);
            }
        }

        void add_conflict_arc(const std::pair<int,int>& ca){
            int i, j;
            std::tie(i,j) = ca;
            add_arc(i, nodes[j].pair, def::MAX_INT);
        }

        size_t num_arcs(){
            size_t num = 0;
            for(const auto& va : arcs){
                num += va.size();
            }
            return num;
        }
    };

}
#endif //H_SDTP_KA_MAX_FLOW_GRAPH_H
