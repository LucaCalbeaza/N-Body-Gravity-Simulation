/** 
 * File: magiGeneration.cpp
 * Description: Implementations for the magiGeneration class.
*/

#include "magiGeneration.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MagiGeneration::MagiGeneration(std::vector<Body>& stars, GUI::inputParameters parameters) {
    stars = magiLoadHdf5(parameters);
}

void MagiGeneration::magiCustomGeneration() {
   //
}

std::vector<Body> MagiGeneration::magiLoadHdf5(GUI::inputParameters parameters) {
    std::vector<Body> stars;
    const float G_REAL = 4.5e-12f; 

    // Compute Unit Constants
    float kpcPerUnit = parameters.genSizeKpc * 0.5f;
    float G = G_REAL * (parameters.billionSolarMass * 1e9f) / (kpcPerUnit * kpcPerUnit * kpcPerUnit);
    float galaxyUnitSize = parameters.genSizeKpc / 7.5f;

    // Compute Scales
    float Lscale = galaxyUnitSize / 2.0f;                      
    float Tscale = std::sqrt((Lscale * Lscale * Lscale) / G); 
    float Vscale = Lscale / Tscale; 


    try {
        H5::H5File file("magiGenerations/testplummer.hdf5", H5F_ACC_RDONLY);

        // Load Datasets
        H5::DataSet positionData  = file.openDataSet("PartType1/Coordinates");
        H5::DataSet velocityData  = file.openDataSet("PartType1/Velocities");
        H5::DataSet massData = file.openDataSet("PartType1/Masses");

        // Find n 
        hsize_t positionDims[1];
        positionData.getSpace().getSimpleExtentDims(positionDims, nullptr);
        size_t n3 = positionDims[0];   
        size_t n = n3 / 3;

        // Read position, velocity and mass from hdf5 files
        std::vector<float> positionVector(n3);
        positionData.read(positionVector.data(), H5::PredType::NATIVE_FLOAT);

        std::vector<float> velocityVector(n3);
        velocityData.read(velocityVector.data(), H5::PredType::NATIVE_FLOAT);

        std::vector<float> massVector(n);
        massData.read(massVector.data(), H5::PredType::NATIVE_FLOAT);

        for (size_t i = 0; i < n; i++) {
            glm::vec3 position = glm::vec3(positionVector[3 * i], positionVector[3 * i + 1], positionVector[3 * i + 2]) * Lscale;
            glm::vec3 veloctiy = glm::vec3(velocityVector[3 * i], velocityVector[3 * i + 1], velocityVector[3 * i + 2]) * Vscale;
            glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
            Body star(position, veloctiy, acceleration, massVector[i]);
            stars.push_back(star);
        }

    } catch (H5::Exception& e) {
        std::cout << "HDF5 error: " << e.getCDetailMsg() << std::endl;
        return stars;
    }
    return stars;
}