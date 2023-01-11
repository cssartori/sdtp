#ifndef H_SDTP_ARC_H
#define H_SDTP_ARC_H

//Basic structure for the arcs of a simple temporal network

#include "def.hpp"

namespace sdtp{
    struct Arc{
        int source;
        int target;
        int weight;

        Arc(int src, int tgt, int wgt){
            source = src;
            target = tgt;
            weight = wgt;
        }

        Arc(){
            source = def::NO_NODE;
            target = def::NO_NODE;
            weight = 0;
        }
    };
}

#endif //H_SDTP_ARC_H
