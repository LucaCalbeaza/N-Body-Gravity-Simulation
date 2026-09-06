/** 
 * File: magiGeneration.cpp
 * Description: Implementations for the magiGeneration class.
*/

#include "magiGeneration.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MagiGeneration::MagiGeneration() {}

void MagiGeneration::launchCustomGen(GUI::inputParameters parameters) {
    // Select MAGI configuration
    std::string configFile = profileConfigs[parameters.magiProfileIndex];
    outputFilename = "livegen-" + configFile;

    // Construct WSL Linux Command
    std::string command =
        "wsl.exe --cd ~/magi/build -- bash -c \""
        "bin/magi -config=single/" + configFile +
        " -file=" + outputFilename +
        " -Ntot=" + std::to_string(parameters.n) +
        " -eps=1.5625e-2 -eta=0.5 -ft=1575.0 -snapshotInterval=25.0 -saveInterval=140.0"
        " && cp dat/" + outputFilename + ".hdf5"
        " /mnt/c/Users/calbe/Documents/GitHub/N-Body-Orbital-Simulation/magiGenerations/" + outputFilename + ".hdf5\"";

    STARTUPINFOA si{ sizeof(si) };
    ZeroMemory(&magiProcessInfo, sizeof(magiProcessInfo));

    // Convert command to char buffer
    std::vector<char> cmdBuffer(command.begin(), command.end());
    cmdBuffer.push_back('\0');

    // Run WSL Process 
    CreateProcessA(
        nullptr, cmdBuffer.data(), nullptr, nullptr, FALSE,
        CREATE_NO_WINDOW, nullptr, nullptr, &si, &magiProcessInfo
    );
    processLaunched = true;
}

bool MagiGeneration::pollComplete() {
    // Check if magiProcessInfo is complete
    if (!processLaunched) return false;
    DWORD exitCode;
    GetExitCodeProcess(magiProcessInfo.hProcess, &exitCode);
    if (exitCode != STILL_ACTIVE) {
        CloseHandle(magiProcessInfo.hProcess);
        CloseHandle(magiProcessInfo.hThread);
        return true;
    }
    return false; 
}

std::vector<Body> MagiGeneration::magiLoadHdf5(std::vector<Body>& stars, GUI::inputParameters parameters) {
    // G value in kpc, solarmass, myr unit system
    const float G_REAL = 4.5e-12f; 
    stars.clear();

    // Compute Scale Conversion
    float kpcPerUnit = parameters.genSizeKpc * 0.5f;
    float G = G_REAL * (parameters.billionSolarMass * 1e9f) / (kpcPerUnit * kpcPerUnit * kpcPerUnit);
    float galaxyUnitSize = parameters.genSizeKpc / 7.5f;
    float Lscale = galaxyUnitSize / 2.0f;                      
    float Tscale = std::sqrt((Lscale * Lscale * Lscale) / G); 
    float Vscale = Lscale / Tscale; 


    try {
        // Load File
        H5::H5File file("magiGenerations/" + outputFilename + ".hdf5", H5F_ACC_RDONLY);

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

        // Create Stars
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