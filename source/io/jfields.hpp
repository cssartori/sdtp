#ifndef H_SDTP_JSON_FIELDS_H
#define H_SDTP_JSON_FIELDS_H

#include <string>

namespace sdtp::jfields{
    namespace instance{
        const static std::string NAME = "name";
        const static std::string NUM_VARIABLES = "num-variables";
        const static std::string NUM_CONSTRAINTS = "num-constraints";
        const static std::string VARIABLES = "variables";
        const static std::string VAR_ID = "id";
        const static std::string VAR_DOMAINS = "domains";
        const static std::string VAR_DOM_LOW = "l";
        const static std::string VAR_DOM_UP = "u";
        const static std::string VAR_DOMAIN_SIZE = "domain-size";
        const static std::string CONSTRAINTS = "constraints";
        const static std::string STATUS = "status";
        const static std::string COMMENT = "comment";
    };

    namespace log{
        const static std::string INSTANCE = "instance";
        const static std::string MESSAGES = "messages";
        const static std::string RESULTS = "results";
        const static std::string RES_METHOD = "method";
        const static std::string RES_STATUS = "status";
        const static std::string RES_MESSAGES = "messages";
        const static std::string RES_SOLUTIONS = "solutions";
        const static std::string RES_VALUES = "values";
        const static std::string RES_FEASIBILITY = "feasibility";
        const static std::string RES_VALS_VAR = "variable";
        const static std::string RES_VALS_VAL = "values";
        const static std::string RES_RUNTIMES = "runtimes";
        const static std::string RES_TIMEOUTS = "timeouts";
        const static std::string RES_SCANS = "scans";
        const static std::string INST_VAR_DISJ = "num-var-disjunct";
        const static std::string INST_MAX_DISJ = "max-disjunct";
        const static std::string INST_SUM_DISJ = "sum-disjunct";
        const static std::string INST_NUM_CONS = "num-constraints";
        const static std::string INST_NUM_VARS = "num-variables";
        const static std::string DATE = "date";
    };

    namespace perf{
        const static std::string INSTANCE = "instance";
        const static std::string INST_VAR_DISJ = "num-var-disjunct";
        const static std::string INST_MAX_DISJ = "max-disjunct";
        const static std::string INST_SUM_DISJ = "sum-disjunct";
        const static std::string INST_NUM_CONS = "num-constraints";
        const static std::string INST_NUM_VARS = "num-variables";
        const static std::string VALUES = "values";
        const static std::string DATE = "date";
        const static std::string PV_SCANS = "scans";
        const static std::string PV_METHOD = "method";
        const static std::string PV_CACHE_HITS = "cache-hits";
        const static std::string PV_CACHE_MISSES = "cache-misses";
        const static std::string PV_L1D_HITS = "l1d-hits";
        const static std::string PV_L1D_MISSES = "l1d-misses";
        const static std::string PV_LL_READ_HITS = "ll-read-hits";
        const static std::string PV_LL_READ_MISSES = "ll-read-misses";
        const static std::string PV_LL_WRITE_HITS = "ll-write-hits";
        const static std::string PV_LL_WRITE_MISSES = "ll-write-misses";
    };
};

#endif // H_SDTP_JSON_FIELDS_H
