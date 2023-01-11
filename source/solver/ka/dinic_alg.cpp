#include "dinic_alg.hpp"

namespace sdtp::ka{

    DinicAlg::DinicAlg(MaxFlowGraph& mfg_) : mfg(mfg_) {
        level.resize(mfg.nodes.size());
        iter.resize(mfg.nodes.size());
        source = mfg.source;
        sink = mfg.sink;
    }

    void DinicAlg::reset(){
        level.assign(mfg.nodes.size(), -1);
        iter.assign(mfg.nodes.size(), 0);
    }

    bool DinicAlg::bfs(int s) {
        q.push(s);
        level[s] = 0;

        while (q.size() > 0) {
            int i = q.front();
            q.pop();

            for(int a=0;a<(int)mfg.arcs[i].size();a++){
                MfArc& arc = mfg.arcs[i][a];
                if (level[arc.to] != -1) continue;

                if (arc.residual_capacity() > 0){
                    level[arc.to] = level[arc.from] + 1;
                    q.push(arc.to);
                }
            }
        }

        return (level[sink] != -1);
    }


    int DinicAlg::dfs(int i) {
        //for the graph with unitary capacities of bi-partide matching, it suffices to find one path to t
        //with residual capacity > 0

        if (i == sink) return 1; //max-flow we can push in each path s-t

        int f = 0;
        bool found = false;
        while(not found && iter[i] < (int)mfg.arcs[i].size()){
            MfArc& arc = mfg.arcs[i][iter[i]];
            int j = arc.to;

            if(arc.residual_capacity() <= 0 || level[i] + 1 != level[j]){
                iter[i]++;
                continue;
            }

            f = dfs(j);
            if (f == 0){
                iter[i]++;
                continue;
            }

            arc.flow += f;
            MfArc& rarc = mfg.arcs[arc.to][arc.revi];
            rarc.flow -= f;
            found = true;
        }

        return f;
    }

    int DinicAlg::run() {
        int flow = 0;
        while (true) {
            level.assign(mfg.nodes.size(), -1);
            bool cont = bfs(source);
            if (not cont) break;

            iter.assign(mfg.nodes.size(), 0);
            int f;
            do{
                f = dfs(source);
                flow += f;
            }while(f > 0);
        }

        return flow;
    }
}
