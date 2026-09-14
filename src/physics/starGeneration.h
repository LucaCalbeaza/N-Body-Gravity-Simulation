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
#include <filesystem>
#include <cctype>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class StarGeneration {
public:
    PROCESS_INFORMATION magiProcessInfo{};
    bool processLaunched = false;
    std::string outputFileName = "DefaultHDF5Name";
    std::vector<std::string> lastComponentOrder;
    std::vector<std::pair<glm::vec3, float>> colorRatios = {
        {glm::vec3(0.616f, 0.706f, 1.0f), 0.0f},        // Type O Star Color : Blue
        {glm::vec3(0.667f, 0.749f, 1.0f), 0.0f},        // Type B Star Color : Light Blue
        {glm::vec3(0.792f, 0.847f, 1.0f), 0.0f},        // Type A Star Color : White
        {glm::vec3(0.984f, 0.973f, 1.0f), 0.0f},        // Type F Star Color : Yellow White
        {glm::vec3(1.0f, 0.961f, 0.925f), 0.0f},        // Type G Star Color : Yellow
        {glm::vec3(1.0f, 0.824f, 0.631f), 0.0f},        // Type K Star Color : Orange
        {glm::vec3(1.0f, 0.745f, 0.498f), 0.0f},        // Type M Star Color : Red
        {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f}};           // Invisible: Black
    
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
     * Returns the directory of the magiGenerations/newGenerations 
     * folder on the current computer. 
     */
    std::string getHDF5OutputDir();

    /**
     * Returns true if the MAGI generation process is complete,
     * returns false otherwise. 
     */
    bool pollComplete();

    /**
     * Annotates each component in the given HDF5 file with a "ComponentCategory"
     * (int) and "ComponentName" (string) attribute, based on lastComponentOrder.
     * Purpose is so that the HDF5 file describes each component within the file with 
     * an label other than just PartTypeN
     */
    void annotateHdf5Components(const std::string& hdf5FilePath, Parameters& parameters);
    
    /**
     * Extracts the initial condtions for the simulation stars 
     * from the HDF5 file of the current outputFilename in the 
     * magiGenerations folder. 
     */
    void magiLoadHdf5(Parameters& parameters);

    /**
     * Generated a color for the star
     */
    glm::vec3 generateColor(int componentType);

};

#endif
