#ifndef H_SDTP_DEF_H
#define H_SDTP_DEF_H

//Global basic definitions for SDTP implementation
#include <limits>
#include <string>

namespace sdtp::def{
    const static int NO_NODE = -1;
    const static int NO_ARC = -1;
    const static int MAX_INT = 100000000;
    const static int MIN_INT = std::numeric_limits<int>::min();
    const static long MAX_LONG = std::numeric_limits<int>::max();

    const static long TIME_LIMIT = 2000000; //2 seconds in microseconds

    const static std::string ALG_CRA_NAME = "CRA";
    const static std::string ALG_KAJ_NAME = "KAJ";
    const static std::string ALG_KAB_NAME = "KAB";
    const static std::string ALG_BFDC_NAME = "BFDC";
    const static std::string ALG_ILP_NAME = "ILP";
    const static std::string ALG_ULT_NAME = "ULT";
    const static std::string ALG_RULT_NAME = "RULT";
    const static std::string ALG_CP_NAME = "CP";
    const static std::string ALG_SCP_NAME = "SCP";
    const static std::string ALG_NONE_NAME = "NONE";
};

#endif //H_SDTP_DEF_H
