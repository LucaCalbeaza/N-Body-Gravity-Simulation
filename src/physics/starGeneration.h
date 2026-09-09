/** 
 * File: starGeneration.h
 * Description: Declarations for the starGeneration class. 
*/

#ifndef STARGENERATION_H
#define STARGENERATION_H


#define _USE_MATH_DEFINES
#include "../physics/body.h"
#include "parameters.h"
#include <H5Cpp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <math.h>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class StarGeneration {
public:
    PROCESS_INFORMATION magiProcessInfo{};
    bool processLaunched = false;
    std::string outputFileName = "DefaultHDF5Name"; 
    
    /**
     * StarGeneration Constructor: Creates StarGeneration
     * object with all default values.
     */
    StarGeneration();


    /**
     * Generates star data based on given initial condition
     */
    void generateStarData(Parameters& parameters);

    /**
     * Adds n random stars to with randomized initial positions and 
     * initial velocity to the simulation.
     */
    void generateUniformDistributionData(Parameters& parameters);

    /**
     * Adds N stars to the simulation generated in 
     * accordance to a Plummer density sphere 
     */
    void generateElipitcalPlummerData(Parameters& parameters);

    
    /**
     * Takes in the given MAGI generation parameters and 
     * runs the MAGI generation through the Linux command. 
     * Resulting HDF5 file is placed in the magiGenerations
     * folder found in the root directory
     */
    void launchCustomGen(Parameters& parameters);

    /**
     * Returns true if the MAGI generation process is complete,
     * returns false otherwise. 
     */
    bool pollComplete();
    
    /**
     * Extracts the initial condtions for the simulation stars 
     * from the HDF5 file of the current outputFilename in the 
     * magiGenerations folder. 
     */
    void magiLoadHdf5(Parameters& parameters);

};

#endif
