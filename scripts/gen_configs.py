import random,os;
import copy;
import math;

mode = "addresses"
##mode = "taxis"

ADD_FILES = {"Berlin":"../addresses/berlin/berlin.txt",
			 "Barcelona":"../addresses/barcelona/barcelona.txt",
			 "Porto-Alegre":"../addresses/porto-alegre/poa.txt",
			 "New-York":"../addresses/new-york/ny.txt"
}

OSM_FILES = {"Berlin":"../../OSM/berlin-data/berlin-latest.osrm",
			 "Barcelona":"../../OSM/spain-data/spain-latest.osrm",
			 "Porto-Alegre":"../../OSM/brazil-data/brazil-latest.osrm",
			 "New-York":"../../OSM/new-york-data/new-york-latest.osrm"
}

MNE_NAMES = {"Berlin":"ber",
			 "Barcelona":"bar",
			 "Porto-Alegre":"poa",
			 "New-York":"nyc"}

CITIES = ["Berlin", "Barcelona", "Porto-Alegre"]
DISTRIBUTIONS = ["cluster", "random", "cluster-random"]
HORIZONS = ["240", "480"] ## minutes
TIME_WINDOWS = ["60", "120"] ## minutes
CAPACITIES = ["100", "300"] ## appears to be a reasonable amount of packets for fedex drivers
DEPOTS = ["central", "random"]

sizes = [100, 200, 400, 600, 800, 1000, 1500, 2000, 2500, 3000, 4000, 5000]
NUM_INST_SIZE = 20
NUM_INST_TOTAL = len(sizes)*NUM_INST_SIZE

def shuffle_options():
	random.shuffle(cities)
	random.shuffle(distributions)
	random.shuffle(horizons)
	random.shuffle(time_windows)
	random.shuffle(capacities)
	random.shuffle(depots)

def cmp_configs(c1, c2):
	cl1 = c1.split(";")
	cl2 = c2.split(";")

	if int(cl1[0]) < int(cl2[0]):
		return -1
	elif int(cl1[0]) > int(cl2[0]):
		return 1
	elif c1 < c2:
		return -1
	elif c1 > c2:
		return 1

	return 0

## function to verify if a given configuration is balanced
def is_config_set_valid(stat, sz):
	valid = True
	i=0

	for x in xrange(0,len(CITIES)):
		if stat[sz][i] < math.ceil(float(NUM_INST_SIZE)/len(CITIES))-1:
			valid = False
		i+=1
		
	for x in xrange(0,len(DISTRIBUTIONS)):
		if stat[sz][i] < math.ceil(float(NUM_INST_SIZE)/len(DISTRIBUTIONS))-1:
			valid = False
		i+=1

	for x in xrange(0,len(HORIZONS)):
		if stat[sz][i] < math.ceil(float(NUM_INST_SIZE)/len(HORIZONS))-1:
			valid = False
		i+=1

	for x in xrange(0,len(TIME_WINDOWS)):
		if stat[sz][i] < math.ceil(float(NUM_INST_SIZE)/len(TIME_WINDOWS))-1:
			valid = False
		i+=1

	for x in xrange(0,len(CAPACITIES)):
		if stat[sz][i] < math.ceil(float(NUM_INST_SIZE)/len(CAPACITIES))-1:
			valid = False
		i+=1

	for x in xrange(0,len(DEPOTS)):
		if stat[sz][i] < math.ceil(float(NUM_INST_SIZE)/len(DEPOTS))-1:
			valid = False
		i+=1
		
	return valid
	
def write_configuration_file(idx, size, city, dist, horizon, tw, cap, depot):
	name="%s-n%d-%d" % (MNE_NAMES[city], size, idx)

	inst = open("../configurations/%s.txt" % (name), "w")
	inst.write("NAME: %s\n" % (name))
	inst.write("SIZE: %d\n" % (size+1)) ## increase one for the depot
	inst.write("DISTRIBUTION: %s\n" % (dist))
	if dist == "cluster" or dist == "cluster-random":
		inst.write("CLUSTERS: %d\n" % (random.randint(3,8)))
		inst.write("DENSITY: %.1f\n" % (random.uniform(0.6, 1.6)))
	inst.write("NODE-FILE: %s\n" % (ADD_FILES[city]))
	inst.write("TIME-WINDOW: %d\n" % (tw))
	if mode == "taxis":
		inst.write("SERVICE-TIME: 2\n") ## about two minutes to get in or off a taxi
	else:
		inst.write("SERVICE-TIME: %d\n" % (random.randint(1,3)*5))
	inst.write("DEPOT: %s\n" % (depot))
	inst.write("ROUTE-TIME: %d\n" % (horizon))
	inst.write("CAPACITY: %d\n" % (cap))
	inst.write("SEED: %d\n" % (random.randint(0,9999)))
	inst.write("OSRM-FILE: %s\n" % (OSM_FILES[city]))
	inst.write("TYPE: PDPTW\n")
	inst.write("LOCATION: %s\n" % city)
	inst.write("COMMENT: Sartori and Buriol (2019)\n")
	inst.write("EOF")
	inst.close()
	

def write_configuration_table(configs):
	print "\\toprule"
	print "ID & Size & City & Distribution & Horizon & Time Windows & Capacities & Depot Location\\\\"
	print "\\hline"
	idx=1
	for x in clst:
		xl = x.replace(";", " & ")
		print "%d & %s\\\\" % (idx, xl)
		idx+=1
	print "\\bottomrule"

def print_list_as_header(a_list):
	for i in xrange(0, len(a_list)):
		print " & %s" % (a_list[i]),
		
def write_configuration_stats(stats, seed):
	print seed
	total = [0 for i in xrange(0, len(cities+distributions+horizons+time_windows+capacities+depots))]
	print "\\toprule"
	print "size",
	print_list_as_header(CITIES)
	print_list_as_header(DISTRIBUTIONS)
	print_list_as_header(HORIZONS)
	print_list_as_header(TIME_WINDOWS)
	print_list_as_header(CAPACITIES)
	print_list_as_header(DEPOTS)
	
	print "\\\\"
	print "\\hline"
	for sz in sizes:
		print "%d " % (sz),
		for i in xrange(0, len(stat[sz])):
			print " & %d" % (stat[sz][i]),
			total[i] += stat[sz][i]
		print "\\\\"

	print "\\hline\n\\textbf{Total} ",
	for t in total:
		print "& %d " % (t),
	print "\\\\"
	print "\\bottomrule"
		

def write_all_config_files(configs):
	idx=1
	csize=0 #current size
	ccity="" #current city
	for x in configs:
		xl = x.split(";")
		if csize != int(xl[0]) or ccity != xl[1]:
			csize = int(xl[0])
			ccity = xl[1]
			idx=1
		write_configuration_file(idx, int(xl[0]), xl[1], xl[2], int(xl[3]), int(xl[4]), int(xl[5]), xl[6])
		idx+=1	

def increase_stat_count(stats, config):
	xl = config.split(";")
	sz = int(xl[0])
	stats[sz][CITIES.index(xl[1])]+=1
	add = len(CITIES)
	stats[sz][DISTRIBUTIONS.index(xl[2])+add]+=1
	add += len(DISTRIBUTIONS)
	stats[sz][HORIZONS.index(xl[3])+add]+=1
	add += len(HORIZONS)
	stats[sz][TIME_WINDOWS.index(xl[4])+add]+=1
	add += len(TIME_WINDOWS)
	stats[sz][CAPACITIES.index(xl[5])+add]+=1
	add += len(CAPACITIES)
	stats[sz][DEPOTS.index(xl[6])+add]+=1
	
def generate_configurations(cities, distributions, horizons, time_windows, capacities, depots, num_inst_size):
	configs = set() ## set to keep configurations generated (make them unique)
	stat = dict() ## map to keep the statistics of configurations for each instance size (such that they are all balanced)
	for sz in sizes:
		stat[sz] = [0 for i in xrange(0, len(cities+distributions+horizons+time_windows+capacities+depots))]

	si=0
	num=0
	lcsz = list() ## list of configurations generated for this size
	while len(configs) < num_inst_size*len(sizes):
		shuffle_options()
		i=0

		while i < len(cities) and num < num_inst_size:
			c = "%d;%s;%s;%s;%s;%s;%s" % (sizes[si], cities[i%len(cities)], distributions[i%len(distributions)], horizons[i%len(horizons)], time_windows[i%len(time_windows)], capacities[i%len(capacities)], depots[i%len(depots)])
			cgens.add("%s;%s;%s;%s;%s;%s" % (cities[i%len(cities)], distributions[i%len(distributions)], horizons[i%len(horizons)], time_windows[i%len(time_windows)], capacities[i%len(capacities)], depots[i%len(depots)]))
			if not c in configs: ## checks if this configuration is unique
				configs.add(c) ## add it to the set of configurations
				lcsz.append(c) ## add it to the list of configurations as well
				num+=1 ## count one instance for this size
			i+=1
			
		if num == num_inst_size:
			for c in lcsz:
				increase_stat_count(stat, c)
			
			if is_config_set_valid(stat, sizes[si]):
				si+=1
			else:
				##print stat[sizes[si]]
				for c in lcsz:
					configs.remove(c)
				stat[sizes[si]] = [0 for i in xrange(0, len(cities+distributions+horizons+time_windows+capacities+depots))]
			num=0
			del lcsz[:]
				
	clst = list()
	for c in configs:
		clst.append(c)
	clst.sort(cmp=cmp_configs)

	return stat, clst

if not os.path.exists("../configurations"):
	os.makedirs("../configurations")

if mode == "taxis":
	NUM_INST_SIZE = 5
	NUM_INST_TOTAL = len(sizes)*NUM_INST_SIZE
	CITIES = ["New-York"]
	DISTRIBUTIONS = ["random"]
	TIME_WINDOWS = ["30", "60", "120"]
	CAPACITIES = ["6"]

cities = copy.deepcopy(CITIES)
distributions = copy.deepcopy(DISTRIBUTIONS)
horizons = copy.deepcopy(HORIZONS)
time_windows = copy.deepcopy(TIME_WINDOWS)
capacities = copy.deepcopy(CAPACITIES)
depots = copy.deepcopy(DEPOTS)


seed = 2
cgens = set()
# while len(cgens) < 216:
# 	seed+=1
# 	if seed % 100 == 0:
# 		print seed
# 	cgens.clear()
# 	random.seed(seed)
# 	stat,clst = generate_configurations(cities, distributions, horizons, time_windows, capacities, depots, NUM_INST_SIZE)

random.seed(seed)
stat,clst = generate_configurations(cities, distributions, horizons, time_windows, capacities, depots, NUM_INST_SIZE)

	
#write_configuration_table(clst)
write_configuration_stats(stat, seed)
#write_all_config_files(clst)

# print len(cgens)
# print seed
# print len(clst)





