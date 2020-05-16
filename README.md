# Open Source Vehicle Routing Instance Generator (OVIG)

The OVIG tool is used to generate Vehicle Routing Problem (VRP) instances based on realistic addresses and travel times. Originally it was used to generate instances for a variant of the VRP called Pickup and Delivery Problem with Time Windows (PDPTW) as part of the Masters' thesis entitled *The Pickup and Delivery Problem with Time Windows: Algorithms, Instances, and Solutions*.

Despite its original intent, the project can be extended to accomodate other VRP variants, such as the *Capacitated Vehicle Routing Problem* (CVRP), the *Vehicle Routing Problem with Time Windows* (VRPTW), and the *Pickup and Delivery Problem* (PDP). Currently this is part of future work.

The project uses Open Street Maps (OSM) from [Geofabrik](http://download.geofabrik.de/) and the [Open Source Routing Machine](https://github.com/Project-OSRM/osrm-backend) (OSRM) tool to compute realistic travel times. Addresses are currently obtained from the [OpenAddresses](https://openaddresses.io/) database, but any data on geographic locations can be used to feed the OVIG tool as long as it is valid.

## Code

The project is thought to be open source, so that anyone interested can contribute to its development, and ideally contribute to the large VRP community. It is developed in `C++` to easily integrate with the OSRM tool. However, the code is currently chaotic (at best!) and I still have to find the time to organize and comment things further. 

## Compiling

So far, the code should compile once all required packages are installed. Other than the basic `C++` compiler (compatible with `C++14`), it is necessary to compile from source and install OSRM, which is easily done by following their guide for *Building from Source*. Once all these dependencies are installed, it is enough to run `make` from the OVIG folder.

## Running

To run the instance generator, one should have a *configuration* text file ready. Then, it is just necessary to call:

```sh
./main -c conf.txt
```

Assuming your configuration file is `conf.txt`. For other options, you may run

```sh
./main -h
```

# Configuration files

To correctly generate an instance a *configuration file* is required, which contains the characteristics desired for the resulting instance. An example is available at `configurations/bar-n100-1.txt`, which may be used to generate an instance in Barcelona containing 100 locations (plus the depot).

The `scripts` can generate the configuration files and the instances for the PDPTW that the tool was primarily developed for. More information about the instances can be found [here](https://github.com/cssartori/pdptw-instances)
