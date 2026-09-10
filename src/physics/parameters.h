/** 
 * File: parameters.h
 * Description: Declarations for the Parameters class. 
*/

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector>
#include <string>
#include <filesystem>
#include "../physics/body.h"  



class Parameters {
public:
    // MAGI component parameters
    struct MagiConfig {
        bool enabled = true;
        std::string name = "Default Name:"; 
        std::string paramFileName; 
        int category = 1.0;
        int magiProfileIndex = 0;
        int componentStarCount = 1; 
        float componentMass = 1.0f; 
        float scaleRadius = 1.0f; 
        float scaleHeight = 1.0f; 
        float extraParam = 1.0f;

        /**
         * magiConfig constructor: sets the given category
         * and name 
         */
        MagiConfig(std::string name, int category);

        /**
         * Reset Config to default parameters apart from 
         * the category and names. Intended to be called 
         * after category is changed.
         */
        void resetConfig();
    };



    // Parameters Fields:

    // Start
    bool startSimulation = false;
    bool startGeneration = false;

    // Window & Camera
    bool simulation3D = true;
    bool window3D = true;
    int cameraCondition = 0;
    
    // Physical Parameters
    int n = 10000;
    float billionSolarMass = 60.0f;
    float genSizeKpc = 15.0f;
    float timeScaleMyrPerSec = 20.0f;
    
    // Computation Method 
    int computationMethod = 0;
    float theta = 0.5;
    
    // Visuals & Render
    int renderMethod = 1;
    float minColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};  
    float maxColor[4] = {1.0f, 1.0f, 1.0f, 1.0f}; 
    float bodyRadius = 0.005f; 

    // Initial Condition
    std::vector<Body> stars;
    int startingCondtion = 0;
    int secondaryStartingCondition = 0;  
    std::vector<MagiConfig> magiParameters;
    std::string generationHDF5FileName = "Generation";

    std::vector<std::string> hdf5FileNameStrings;
    std::vector<const char*> HDF5FileNames;
    int selectedHDF5FileIndex = 0;


    /**
     * Input Parameters constructor: Loads the magiParmeters 
     * vectors with the 7 default components.  
     */
    Parameters();

    /**
     * Scans folderPath for .hdf5 files and adds the names of
     * any .hdf5 files found to hdftFileNameStrings.
     */
    void updateHDF5FileNames();

};


#endif