# Open Source Vehicle Routing Instance Generator (OVIG)

The OVIG tool is used to generate Vehicle Routing Problem (VRP) instances based on realistic addresses and travel times. Originally it was used to generate instances for a variant of the VRP called Pickup and Delivery Problem with Time Windows (PDPTW) as part of the Masters' thesis entitled *The Pickup and Delivery Problem with Time Windows: Algorithms, Instances, and Solutions*.

Despite its original intent, the project can be extended to accomodate other VRP variants, such as the *Capacitated Vehicle Routing Problem* (CVRP), the *Vehicle Routing Problem with Time Windows* (VRPTW), and the *Pickup and Delivery Problem* (PDP). Currently this is part of future work.

The project uses Open Street Maps (OSM) from [Geofabrik](http://download.geofabrik.de/) and the [Open Source Routing Machine](https://github.com/Project-OSRM/osrm-backend) (OSRM) tool to compute realistic travel times. Addresses are currently obtained from the [OpenAddresses](https://openaddresses.io/) database, but any data on geographic locations can be used to feed the OVIG tool as long as it is valid.

## Code

The project is thought to be open source, so that anyone interested can contribute to its development, and ideally contribute to the large VRP community. It is developed in `C++` to easily integrate with the OSRM tool. However, the code is currently chaotic (at best!) and I still have to find the time to organize and comment things further. 

So far, it should compile once all required packages are installed. The `scripts` can generate the configuration files and the instances of the PDPTW that the tool was primarily developed for. More information about the instances can be found [here](https://github.com/cssartori/pdptw-instances)
