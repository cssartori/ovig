# Open Source Vehicle Routing Instance Generator (OVIG)

The OVIG tool was designed to generate Vehicle Routing Problem (VRP) instances based on realistic addresses and travel times. For that, the tool uses maps from OpenStreetMap (OSM) available, for example, at [Geofabrik](http://download.geofabrik.de/). To compute travel times over these maps, OVIG employs the [Open Source Routing Machine](https://github.com/Project-OSRM/osrm-backend) (OSRM). Addresses (or geographic locations) can be obtained from the [OpenAddresses](https://openaddresses.io/) database, however any other dataset on geographic locations can be used to feed the OVIG tool as long as it is valid.

Originally, the OVIG was implemented to generate instances for a VRP variant known as the *Pickup and Delivery Problem with Time Windows* (PDPTW). The description of the method used by the tool and the reasoning behind it can be found as part of a [scientific paper](https://doi.org/10.1016/j.cor.2020.105065) published in the journal *Computers & Operations Research* (2020), as well as part of the Masters' thesis in Computer Science (2019) available [here](https://www.lume.ufrgs.br/handle/10183/194380).

Despite its original intent, the project can be extended to accomodate other VRP variants, such as the *Capacitated Vehicle Routing Problem* (CVRP), the *Vehicle Routing Problem with Time Windows* (VRPTW), and the *Pickup and Delivery Problem* (PDP). Currently, OVIG can produce instances for the CVRP and PDPTW. The addition of other instance types is still open and may receive contributions of interested users.

The figures below illustrate three instances generated using OVIG. All of them have 100 locations selected using different types of probability distributions. Barcelona and Denmark are `random` while Berlin is `cluster-random`. Note that in all instances, the customer distribution still follows an underlying unspecified function due to how the data is provided. For example, locations in Barcelona are mostly distributed within an ellipsoid between two very unpopulated areas (of land and sea). Images generated using [GPSVisualizer](https://www.gpsvisualizer.com/).

| <img src="https://github.com/cssartori/ovig/blob/master/docs/figures/fig_barcelona.png" width="300" height="300"> | <img src="https://github.com/cssartori/ovig/blob/master/docs/figures/fig_berlin.png" width="300" height="300"> | <img src="https://github.com/cssartori/ovig/blob/master/docs/figures/fig_denmark.png" width="300" height="300">
|:--:|:--:|:--:|
| *Barcelona* | *Berlin* | *Denmark*| 



## Building

**Prerequesits:** Here is a list of what needs to be done *before* compiling OVIG.
1. C++ compiler compatible with at least C++14: the recommendation is to use GNU GCC version 7.5 or later;
2. CMAKE: version 3.10 or later. More information [here](https://cmake.org/);
3. Boost C++ library: recommended version 1.65 or later. More information [here](https://www.boost.org/);
4. OSRM: compiled and installed from source by following their [guide](https://github.com/Project-OSRM/osrm-backend#building-from-source). Recommended version is [5.25](https://github.com/Project-OSRM/osrm-backend/releases/tag/v5.25.0).

Once these dependencies have been correctly installed in your system, OVIG can be compilled. If you are using some Ubuntu (or similar Linux distributions) the following commands should suffice to compile the tool. Assuming you are inside the `ovig` folder.

```sh
mkdir build
cd build
cmake ..
make
```
This will create an executable file called `ovig` inside the `build/` directory.
If you are using another operating system the steps to compile can change substantially. However, I have only compiled within Ubuntu systems myself. Should anyone be interested in setting a configuration to compile in MacOS / Windows, please feel free to do so.

## Preprocessing maps

Before running OVIG to generate instances, it is necessary to preprocess maps with OSRM to create `.osrm` files, which will be then used to compute travel time (or distance) matrices for our instances. To do so, you may use the script available within `scrpits/preproc.sh`, which may be called (assuming you are in the `scripts/` directory):
```sh
./preproc.sh ../../osrm-backend/profiles/car.lua ../../osm/berlin-latest.osm.pbf
```
The first argument is the path to a OSRM profile file (in this case the one for cars) and the second argument provides the basic OSM file. The car profile is included in the OSRM repository by default. Meanwhile, the Berlin OSM data can be downloaded from [Geofbarik](http://download.geofabrik.de/europe/germany/berlin.html).

After preprocessing is done, a file called `berlin-latest.osrm` (note the change in extension) will be available for use within the `../../osm/` folder (or whichever folder the original OSM file was located in). This file will be used by OVIG afterwards.

**Important**: the `car.lua` profile that is provided with OSRM v5.25 is, by default, configured to route using `routability` as metric which does not guarantee that the time/distance matrix will respect the [triangular inequality](https://en.wikipedia.org/wiki/Triangle_inequality). Since most VRPs assume that the input matrix has such property, it is recommended to change the configuration `weight_name` inside the `car.lua` profile to `duration` or `distance` instead of `routability`. Note that even if this is not modified, OVIG will try to fix the resulting matrix to make it respect the triangular inequality. However, the ideal scenario is to avoid such fix and produce the correct matrix from OSRM to begin with.

## Configuration files

OVIG receives a single *configuration file* as a parameter to generate an instance. The configuration file describes the characteristics that the produced instace will have, including: number of nodes, probability distribution for selection of the nodes, time window width, time horizon and vehicle capacity. More information can be found in the `configurations/` folder. 

Two examples are available:
1. `configurations/cvrp-n100-ber.txt`: produces a CVRP instance with 100 nodes (plus one depot) with random probability to select nodes and vehicle capacity of 20 units. Nodes are selected in Berlin among the locations provided in the file `addresses/berlin/berlin.txt`;
2. `configurations/pdptw-n100-ber.txt`: produces a PDPTW instance with 100 nodes (plus one depot), or 50 paired requests, with cluster-random probability to select nodes using 3 clusters and 1.2 density. Time window width is 60 minutes while the time horizon is 480 minutes. Vehicle capacity is set to 100 units. Addresses are also selected from `addresses/berlin/berlin.txt`.

The python scripts inside the `scripts/` directory can generate all the configuration files for the PDPTW instances that the tool was primarily developed for. More information about these PDPTW instances can be found [here](https://github.com/cssartori/pdptw-instances).

## Running

Given that all previous steps have been completed. An instance can be generated by running (assuming you are in the `build/` directory):

```sh
./ovig -c ../configurations/pdptw-n100-ber.txt -o ipdptw-n100-ber.txt
```
Creating the instance file `ipdptw-n100-ber.txt` inside the `build/` directory.

For more options, you may run:

```sh
./ovig -h
```

## Code

The project is completely open source, so anyone interested can contribute to its development and to the VRP community. It is developed in `C++` to easily integrate with the OSRM tool. However, the code currently lacks some documentation and new features. If you are interested in contributing, some enhancement proposals may be found in this repository. 

