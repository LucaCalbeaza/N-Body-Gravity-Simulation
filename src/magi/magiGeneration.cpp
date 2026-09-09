/** 
 * File: magiGeneration.cpp
 * Description: Implementations for the magiGeneration class.
*/

#include "magiGeneration.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MagiGeneration::MagiGeneration() {}

void MagiGeneration::launchCustomGen(GUI::InputParameters& parameters) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(1, 100000);
    int randomNameID = distrib(gen);

    // Filename and Output directory
    outputFileName = parameters.hdf5FileName + std::to_string(randomNameID);
    std::string hdf5OutputDir =
        "/mnt/c/Users/calbe/Documents/GitHub/N-Body-Orbital-Simulation/magiGenerations/newGenerations";

    // Default Parameter Options
    const float DISK_RADIAL_DISPERSION = -1.0f; 
    const float DISK_TOOMRE_Q          = 1.8f;
    const float DISK_RETROGRADE_FRAC   = 0.0f;

    std::string commandScript = "mkdir -p cfg/generated && mkdir -p " + hdf5OutputDir + " && ";

    int numberOfComponents = parameters.magiParameters.size();

    // Create Param Files in the MAGI directory for each component
    for (int i = 0; i < numberOfComponents; i++) {
        GUI::MagiConfig& config = parameters.magiParameters[i]; 
        if (config.enabled) {
            std::string paramFileName = outputFileName + "-C" + std::to_string(i);
            config.paramFileName = paramFileName;
            commandScript += "cat > cfg/generated/" + paramFileName + ".param << 'EOF'\n";


            if (config.category == 0) {
                // Spherodial Case (Bulges & Halos)
                float cutoffRadius = config.scaleRadius * 15.0f;
                float cutoffWidth  = cutoffRadius * 0.08f;
                commandScript += std::to_string(config.componentMass / parameters.billionSolarMass) + "\n";
                commandScript += std::to_string(config.scaleRadius) + "\n";
                // King or Einasto spheres require an extra parameter
                if (config.magiProfileIndex == 1 || config.magiProfileIndex == 6) {
                    commandScript += std::to_string(config.extraParam) + "\n";
                }
                commandScript += "1\n";
                commandScript += std::to_string(cutoffRadius) + " " + std::to_string(cutoffWidth) + "\n";

            } else if (config.category == 1) {
                // Disk Case
                float cutoffRadius = config.scaleRadius * 15.0f;
                float cutoffWidth  = cutoffRadius * 0.08f;
                commandScript += std::to_string(config.componentMass / parameters.billionSolarMass) + "\n";
                commandScript += std::to_string(config.scaleRadius) + "\n";
                commandScript += std::to_string(config.scaleHeight) + "\n";
                // Sersic disks require an extra parameter
                if (config.magiProfileIndex == -2) { 
                    commandScript += std::to_string(config.extraParam) + "\n";
                }
                commandScript += std::to_string(DISK_RADIAL_DISPERSION) + " " + std::to_string(DISK_TOOMRE_Q) + "\n";
                commandScript += std::to_string(DISK_RETROGRADE_FRAC) + "\n";
                commandScript += "1\n"; 
                commandScript += std::to_string(cutoffRadius) + " " + std::to_string(cutoffWidth) + "\n";

            } else if (config.category == 2) {
                // Central Massive Blackhole case
                commandScript += std::to_string(config.componentMass / parameters.billionSolarMass) + "\n";
            }

            commandScript += "EOF\n";
        }
    }

    // Create Config File in the MAGI directory
    commandScript += "cat > cfg/generated/" + outputFileName + ".cfg << 'EOF'\n";
    commandScript += "-1\n";
    int activeComponents = 0;
    for (int i = 0; i < numberOfComponents; i++) {
        if (parameters.magiParameters[i].enabled) {
            activeComponents++;
        }
    }
    commandScript += std::to_string(activeComponents) + "\n";
    for (int i = 0; i < numberOfComponents; i++) {
        GUI::MagiConfig config = parameters.magiParameters[i]; 
        if (config.enabled) {
            int profileIndex = 0;
            if (config.category == 0) {
                profileIndex = config.magiProfileIndex;
            } else if (config.category == 1) {
                profileIndex = (config.magiProfileIndex == 0) ? -1 : -2;
            } else if (config.category == 2) {
                profileIndex = 1000;
            }
            commandScript += std::to_string(profileIndex) + " generated/" + config.paramFileName + ".param 1 " +
                    std::to_string(config.componentStarCount) + "\n";
        }
    }
    commandScript += "EOF\n"; 

    // Construct WSL Linux Command
    commandScript += "bin/magi -config=generated/" + outputFileName + ".cfg -file=" + outputFileName +
               " -Ntot=" + std::to_string(parameters.n) +
               " -eps=1.5625e-2 -eta=0.5 -ft=1575.0 -snapshotInterval=25.0 -saveInterval=140.0 && ";
    commandScript += "cp dat/" + outputFileName + ".hdf5 " + hdf5OutputDir + "/" + outputFileName + ".hdf5 && ";
    commandScript += "echo done";
    std::string command = "wsl.exe --cd ~/magi/build -- bash -c \"" + commandScript + "\"";

    // Start up process
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

std::vector<Body> MagiGeneration::magiLoadHdf5(std::vector<Body>& stars, GUI::InputParameters parameters) {
    // G value in kpc, solarmass, myr unit system
    const float G_REAL = 4.5e-12f; 
    stars.clear();

    // Compute Total CutoffRadius
    float maxCutoffRadius = 0.0f;
    for (int i = 0; i < parameters.magiParameters.size(); i++) {
        if (parameters.magiParameters[i].category != 2 && parameters.magiParameters[i].enabled) {
            maxCutoffRadius = std::max(maxCutoffRadius, parameters.magiParameters[i].scaleRadius * 15.0f);
        }
    }

    // Compute Scale Conversion
    float kpcPerUnit = parameters.genSizeKpc * 0.5f;
    float G = G_REAL * (parameters.billionSolarMass * 1e9f) / (kpcPerUnit * kpcPerUnit * kpcPerUnit);
    float galaxyUnitSize = parameters.genSizeKpc / 7.5f;
    float Lscale = (galaxyUnitSize / 2.0f) / maxCutoffRadius;                      
    float Tscale = std::sqrt((Lscale * Lscale * Lscale) / G); 
    float Vscale = Lscale / Tscale; 

    try {
        // Load File
        H5::H5File file("magiGenerations/newGenerations/" + outputFileName + ".hdf5", H5F_ACC_RDONLY);

        // Divide into component sub groups
        H5::Group root = file.openGroup("/");
        hsize_t numberOfComponents = root.getNumObjs();

        // Load initial condition data from each component
        for (hsize_t i = 0; i < numberOfComponents; i++) {
            std::string name = root.getObjnameByIdx(i);
            if (name.rfind("PartType", 0) != 0) {
                continue;
            }

            // Load Datasets
            H5::DataSet positionData  = file.openDataSet(name + "/Coordinates");
            H5::DataSet velocityData  = file.openDataSet(name + "/Velocities");
            H5::DataSet massData = file.openDataSet(name + "/Masses");

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
        }
    } catch (H5::Exception& e) {
        std::cout << "HDF5 error: " << e.getCDetailMsg() << std::endl;
        return stars;
    }

    return stars;
}