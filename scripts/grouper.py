import sys, os, argparse;
from operator import itemgetter;
import subprocess;
import json;
import time;
import atexit;
import math;


HEADER_TIME = "Size;Method;Time;Status;Solution;Timeout;Scans"
HEADER_CACHE = "Instance;Size;Method;Value;Metric"

## to remove some algorithms from the plots!
FILTER_OUT_ALGS = set(["KAJ"]) ## here we are removing KAJ only
#FILTER_OUT_ALGS = set([])

class AlgTimeResult:
    def __init__(self, name_):
        self.name = name_
        self.runtimes = list()
        self.status = list()
        self.solutions = list()
        self.timeouts = list()
        self.scans = list()

    def merge(self, runtimes_, status_, solutions_, timeouts_, scans_):
        self.runtimes += runtimes_
        self.status += status_
        self.solutions += solutions_
        self.timeouts += timeouts_
        self.scans += scans_

    def kth_entry_to_str(self, k, sep=';'):
        name = self.name
        csv = ("%s%c%d%c%s%c%s%c%s%c%.2f" % (name, sep, self.runtimes[k], sep, 1 if self.status[k] else 0, sep, 1 if self.solutions[k] else 0, sep, 1 if self.timeouts[k] else 0, sep, self.scans[k] ))
        return csv

    def all_timeouts(self):
        to = self.timeouts.count(True)
        return (to == len(self.timeouts))

    def any_timeout(self):
        to = self.timeouts.count(True)
        return (to >= 1)

class AlgCacheResult:
    def __init__(self, name_):
        self.name = name_
        self.hits = list()
        self.misses = list()

    def merge(self, hit, miss):
        self.hits.append(hit)
        self.misses.append(miss)

    def kth_entry_to_str(self, k, sep=';'):
        csv = ("%s%c%d%c%d" % (self.name, sep, self.hits[k], sep, self.misses[k]))
        return csv


class GroupTimeResult:
    def __init__(self):
        self.data = dict()

    def add(self, alg_name, runtimes, status, solutions, timeouts, scans):
        if len(runtimes) != len(status) or len(status) != len(solutions) or len(solutions) != len(timeouts) or len(scans) != len(timeouts):
            raise ValueError('Length of arrays differ!')

        if not alg_name in self.data:
            self.data[alg_name] = AlgTimeResult(alg_name)

        self.data[alg_name].merge(runtimes, status, solutions, timeouts, scans)


class GroupCacheResult:
    def __init__(self, size):
        self.data = dict()
        self.size = size

    def add(self, alg_name, hit, miss):
        if not alg_name in self.data:
            self.data[alg_name] = AlgCacheResult(alg_name)

        self.data[alg_name].merge(hit, miss)


# #Prepare the arguments the program shall receive
def __prepareargs__():
    parser = argparse.ArgumentParser(description='Parse result files from a folder to new file')
    parser.add_argument('-d', nargs=1, type=str, help='Base directory with results', required=True)
    parser.add_argument('-t', nargs=1, type=str, help="Type of graph. One of: 'nodes', 'density', 'num-disjunctions' or 'var-disjunctions'", required=True)
    parser.add_argument('-o', nargs=1, type=str, help="Output file path.", required=True)
    parser.add_argument('-m', nargs=1, type=str, help="Mode of files to process: 'time' or 'cache'.", required=True)
    parser.add_argument('--grid', dest='grid', action='store_true')
    parser.set_defaults(grid=False)

    return parser

# #Parse the input arguments and returns a dictionary with them
def __getargs__(parser):
	args = vars(parser.parse_args())
	return args

# # Get all files (recursively) under folder direc ending with ext(ension)
def get_list_of_files(direc, ext=""):
	files = list() ## a list of all files
	dir_content = os.listdir(direc)

	for c in dir_content:
		path = os.path.join(direc, c)
		if os.path.isdir(path): ## check if it is a directory
			## call the function in the directory to get its files
			files += get_list_of_files(path, ext)
		elif not ext or c.endswith(ext):
			files.append(path)

	return files

def get_num_arcs(n, grid):
    ##n=n-1
    if not grid:
        return (n*n-n)
    else:
        x=math.floor(n/16)
        y=16
        a = x*y*3
        m = a + (y*y-y)*x
        return m


def read_time_files(all_files, itype, grid):
    results = dict()
    for filename in all_files:
        with open(filename, "r") as log:
            data = json.load(log)

            if data["messages"] != "ok":
                print("WARNING: possible error in file '%s' with message '%s'" % (filename, data["message"]))

            if itype == "nodes":
                n = int(data["num-variables"])
                x = n
            elif itype == "density":
                n = int(data["num-variables"])
                m = int(data["num-constraints"])
                maxa = get_num_arcs(n, grid)
                d = float("%.4f" % ((m*1.0)/(maxa)))
                d = round(d*100.0)/100.0
                if(d < 0.2):
                    print("Density for %s is not expected: %.6f" % (filename, d))
                x = d
            elif itype == "num-disjunctions":
                x = int(data["max-disjunct"])
            elif itype == "var-disjunctions":
                n = int(data["num-var-disjunct"])
                x = n*1.0/int(data["num-variables"])

            if not x in results:
                results[x] = GroupTimeResult()

            for jalg in data["results"]:
                name = jalg["method"]
                results[x].add(name, jalg["runtimes"], jalg["feasibility"], jalg["solutions"], jalg["timeouts"], jalg["scans"])

    return results

def read_cache_files(all_files, itype, grid):
    results = dict()
    for filename in all_files:
        with open(filename, "r") as log:
            data = json.load(log)

            name = data["instance"]
            size = data["num-variables"]
            if not name in results:
                results[name] = GroupCacheResult(size)

            for jalg in data["values"]:
                method = jalg["method"]
                results[name].add(method, jalg["cache-hits"], jalg["cache-misses"])

    return results

def write_time_data(outpath, results):
    global HEADER_TIME

    print("writing to %s" % (outpath))
    with open(outpath, "w") as outf:
        outf.write("%s\n" % (HEADER_TIME))
        for x in results.keys():
            for a in results[x].data.keys():
                if a in FILTER_OUT_ALGS:
                    continue

                for i in range(0, len(results[x].data[a].runtimes)):
                    if results[x].data[a].timeouts[i] and results[x].data[a].all_timeouts():
                        continue

                    if type(x) is float:
                        outf.write("%.4f;%s\n" % (x, results[x].data[a].kth_entry_to_str(i)))
                    else:
                        outf.write("%d;%s\n" % (x, results[x].data[a].kth_entry_to_str(i)))

def write_cache_data(outpath, results):
    global HEADER_CACHE

    print("writing to %s" % (outpath))
    with open(outpath, "w") as outf:
        outf.write("%s\n" % (HEADER_CACHE))
        for x in results.keys():
            for a in results[x].data.keys():
                if a in FILTER_OUT_ALGS:
                    continue
                for i in range(0, len(results[x].data[a].hits)):
                    name = a
                    outf.write("%s;%d;%s;%d;Cache Hits\n" % (x, results[x].size, name, results[x].data[a].hits[i]))
                    outf.write("%s;%d;%s;%d;Cache Misses\n" % (x, results[x].size, name, results[x].data[a].misses[i]))


if __name__ == '__main__':
    #receive and prepare the arguments
    parser = __prepareargs__()
    args = __getargs__(parser)

    #read parameters
    direc = args['d'][0] # directory with results
    itype = args['t'][0] # type of processing
    outp = args['o'][0] # output file path
    grid = args['grid'] # whether the instance is Grid
    mode = args['m'][0]
    ext = ".json"

    if itype != "nodes" and itype != "density" and itype != "num-disjunctions" and itype != "var-disjunctions":
        print("Unknown processing type '%s'" % (itype))
        exit(-1)
    if mode != "time" and mode != "cache":
        print("Unknown mode type '%s" % (mode))
        exit(-1)

    all_files = get_list_of_files(direc, ext)

    if mode == "time":
        results = read_time_files(all_files, itype, grid)
        write_time_data(outp, results)
    else:
        results = read_cache_files(all_files, itype, grid)
        write_cache_data(outp, results)

    print("DONE")
