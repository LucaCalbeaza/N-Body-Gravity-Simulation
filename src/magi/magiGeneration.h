/** 
 * File: magiGeneration.h
 * Description: Declarations for the magiGeneration class. 
*/

#ifndef MAGIGENERATION_H
#define MAGIGENERATION_H

#include "../physics/body.h"
#include "../graphics/gui.h"
#include <H5Cpp.h>
#include <iostream>
#include <vector>
#include <windows.h>

class MagiGeneration {
public:
    PROCESS_INFORMATION magiProcessInfo{};
    bool processLaunched = false;
    std::string outputFileName = "DefaultHDF5Name"; 
    
    // Config Options
    const std::vector<std::string> profileConfigs = {
        "plummer.cfg", "king.cfg", "burkert.cfg", "hernquist.cfg",
        "nfw.cfg", "moore.cfg", "einasto.cfg"
    };
    
    /**
     * MagiGeneration Constructor: Creates MagiGeneration 
     * object with all default values.
     */
    MagiGeneration();

    /**
     * Takes in the given MAGI generation parameters and 
     * runs the MAGI generation through the Linux command. 
     * Resulting HDF5 file is placed in the magiGenerations
     * folder found in the root directory
     */
    void launchCustomGen(GUI::InputParameters& parameters);

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
    std::vector<Body> magiLoadHdf5(std::vector<Body>& stars, GUI::InputParameters parameters);

};

#endif
