import sys, os, argparse;
from operator import itemgetter;
import subprocess;
import json;
import statistics;
import time;
import atexit;
import math;

class AlgSummary:
    def __init__(self, name_):
        self.name = name_
        self.runtimes = list()
        self.timeouts = list()
        self.scans = list()

    def merge(self, runtimes_, timeouts_, scans_):
        self.runtimes += runtimes_
        self.timeouts += timeouts_
        self.scans += scans_

    def max_time(self):
        mx = 0
        for x in range(0,len(self.runtimes)):
            if not self.timeouts[x]:
                mx = max(mx, self.runtimes[x])
        return mx##max(self.runtimes)

    def min_time(self):
        return min(self.runtimes)

    def std_time(self):
        return statistics.stdev(self.runtimes)

    def avg_time(self):
        return statistics.mean(self.runtimes)

    def total_time_ms(self):
        return sum(self.runtimes)/1000.0

    def total_time_s(self):
        return sum(self.runtimes)/1000000.0

    def num_timeouts(self):
        return self.timeouts.count(True)

    def length(self):
        return len(self.timeouts)


# #Prepare the arguments the program shall receive
def __prepareargs__():
    parser = argparse.ArgumentParser(description='Print summary of the results.')
    parser.add_argument('-d', nargs=1, type=str, help='Base directory with results', required=True)

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

def kra_false_negatives(kra, mdbf):
    c = 0
    t = 0
    if(len(kra) != len(mdbf)):
        return 0,0

    for x in range(0,len(mdbf)):
        if mdbf[x] == True:
            t += 1
            if kra[x] == False:
                c += 1

    return c,t


def read_files(all_files):
    results = dict()
    kfnb = 0
    kfnj = 0
    totalb = 0
    totalj = 0
    for filename in all_files:
        with open(filename, "r") as log:
            data = json.load(log)

            if data["instance"][0] == "v":
                continue
            if data["messages"] != "ok":
                print("WARNING: possible error in file '%s' with message '%s'" % (filename, data["message"]))

            kra = list()
            mdbf = list()
            for jalg in data["results"]:
                name = jalg["method"]

                if not name in results:
                    results[name] = AlgSummary(name)

                results[name].merge(jalg["runtimes"], jalg["timeouts"], jalg["scans"])

    return results

def print_summary(results):
    for alg in ["ILP", "CP", "SCP", "ULT", "KAB", "KAJ", "CRA", "RULT", "BFDC"]:
        asum = results[alg]
        print("%s & %.0f & %.0f & %.0f & %.0f & %.2f\\\\" % (alg, asum.max_time()/1000.0, asum.avg_time()/1000.0, asum.std_time()/1000.0, asum.total_time_s(), asum.num_timeouts()*100.0/(asum.length()) ))

if __name__ == '__main__':
    #receive and prepare the arguments
    parser = __prepareargs__()
    args = __getargs__(parser)

    #read parameters
    direc = args['d'][0] #directory with results
    ext = ".json"

    all_files = get_list_of_files(direc, ext)
    results = read_files(all_files)
    print_summary(results)

    print("DONE")
