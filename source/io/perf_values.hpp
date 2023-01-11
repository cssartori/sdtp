#ifndef H_SDTP_PERF_VALUES_H
#define H_SDTP_PERF_VALUES_H

#include <cstdlib>
#include <cstdint>
#include <string>

struct PerfValue{
    uint64_t hits;
    uint64_t misses;

    PerfValue(){
        misses = 0;
        hits = 0;
    }

    PerfValue(uint64_t refs, uint64_t miss){
        misses = miss;
        hits = refs - miss;
    }

    double miss_rate() const{
        return 100*(misses/(misses+hits));
    }

    uint64_t total_references() const{
        return hits+misses;
    }
};

struct PerfValues{
    std::string name;
    PerfValue cache;
    PerfValue l1d;
    PerfValue llc_read;
    PerfValue llc_write;
    double scans{0};

    PerfValues(std::string name_) : name(name_) { }

    void print() const{
        printf("%s\n", name.c_str());
        printf("\tCACHE\t(hits: %lu)\t(misses: %lu)\t(rate: %.2f%%)\n", cache.hits, cache.misses, cache.miss_rate());
        printf("\tL1D-READS\t(hits: %lu)\t(misses: %lu)\t(rate: %.2f%%)\n", l1d.hits, l1d.misses, l1d.miss_rate());
        printf("\tLLC-READS\t(hits: %lu)\t(misses: %lu)\t(rate: %.2f%%)\n", llc_read.hits, llc_read.misses, llc_read.miss_rate());
        printf("\tLLC-WRITE\t(hits: %lu)\t(misses: %lu)\t(rate: %.2f%%)\n", llc_write.hits, llc_write.misses, llc_write.miss_rate());
    }
};

#endif //H_SDTP_PERF_VALUES_H
