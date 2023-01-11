#ifndef H_SDTP_KA_DINIC_ALG_H
#define H_SDTP_KA_DINIC_ALG_H

#include "max_flow_graph.hpp"
#include <vector>
#include <queue>

namespace sdtp::ka{
    class DinicAlg {
    private:
        MaxFlowGraph& mfg;
        int source, sink;
        std::vector<int> level, iter;
        std::queue<int> q;

    public:
        DinicAlg(MaxFlowGraph& mfg_);
        void reset();
        bool bfs(int s);
        int dfs(int i);
        int run();
    };
}

#endif //H_SDTP_KA_DINIC_ALG_H
