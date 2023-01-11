#ifndef H_SDTP_INPUT_H
#define H_SDTP_INPUT_H

#include <iostream>
#include "../common/def.hpp"

namespace sdtp{
    class Input{
    public:
        bool verbose = false;
        long time_limit = def::TIME_LIMIT;
        int num_repetitions = 1;
        std::string instance_path;
        std::string logger_path;
    };
}

#endif //H_SDTP_INPUT_H
