#include "two_ways_bf_alg.hpp"

namespace sdtp::rult{

    TwoWaysBfAlg::TwoWaysBfAlg(const DistGraph& g_dir, const DistGraph& g_rev)
        : dir_graph(g_dir), rev_graph(g_rev), queue(dir_graph.num_nodes()+1)
    {
        dd.resize(g_dir.num_nodes(), false);
        dr.resize(g_dir.num_nodes(), false);
        path_d.assign(dir_graph.num_nodes(), util::LenParData());
        path_r.assign(dir_graph.num_nodes(), util::LenParData());
        scans = 0;
    }

    bool TwoWaysBfAlg::solve(std::vector<int>& dist_d, std::vector<int>& dist_r){
        initialize(dist_d, dist_r);
        bool valid = compute_distances(dir_graph, path_d, dist_d);
        if(valid) valid = compute_distances(rev_graph, path_r, dist_r);
        return valid;
    }

    void TwoWaysBfAlg::initialize(std::vector<int>& dist_d, std::vector<int>& dist_r){
        const DistGraph& graph = dir_graph;
        scans = 0;
        queue.reset();
        queue.push(graph.node_zero());
        path_d[graph.node_zero()].len = 0;
        path_r[graph.node_zero()].len = 0;
        path_d[graph.node_zero()].par = graph.node_zero();
        path_r[graph.node_zero()].par = graph.node_zero();
        dist_d[graph.node_zero()] = 0;
        dist_r[graph.node_zero()] = 0;
    }

    bool TwoWaysBfAlg::compute_distances(const DistGraph& graph, std::vector<util::LenParData>& path, std::vector<int>& dist){
        queue.reset();
        queue.push(graph.node_zero());

        bool valid = true;
        uint32_t iter = 0;

        while(not queue.is_empty() && valid){
            int n = queue.pop();
            if(not queue.has_element(path[n].par)) valid = relax(graph, n, path, dist);
            iter++;
        }

        return valid;
    }

    bool TwoWaysBfAlg::relax(const DistGraph& graph, int i, std::vector<util::LenParData>& path, std::vector<int>& dist){
        scans++;
        for(const Arc& a : graph.arcs[i]){
            int j = a.target;
            int w = a.weight;

            int dj = dist[i] + w;
            if(dj < dist[j]){
                dist[j] = dj;

                if(j == graph.node_zero()) return false; //negative cycle
                path[j].len = path[i].len + 1;
                if(path[j].len >= static_cast<int>(graph.num_nodes())) return false; //negative cycle
                path[j].par = i;

                if(not queue.has_element(j)){
                    queue.push(j);
                }
            }
        }
        return true;
    }

    long TwoWaysBfAlg::get_num_scans() const{
        return scans;
    }
}
