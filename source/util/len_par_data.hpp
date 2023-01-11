#ifndef H_SDTP_UTIL_LEN_PAR_DATA_H
#define H_SDTP_UTIL_LEN_PAR_DATA_H

namespace sdtp::util{
    struct LenParData{
        int len; //length up to node
        int par; //parent of the node

        LenParData(){
            len = 0;
            par = -1;
        }
    };
}

#endif //H_SDTP_UTIL_LEN_PAR_DATA_H
