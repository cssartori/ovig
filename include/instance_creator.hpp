#ifndef __H_INSTANCE_CREATOR_H__
#define __H_INSTANCE_CREATOR_H__

#include "configurations.hpp"
#include "instance.hpp"

/*
 * This namespace holds the whole process to create an instance. 
 * The create() function calls the corresponding Builder function for the particular instance type that is provided
 * in the configuration parameter.
 *
 */

namespace InstanceCreator{
	int create(const Configurations& con, Instance& inst);

};

#endif //__H_INSTANCE_CREATOR_H__
