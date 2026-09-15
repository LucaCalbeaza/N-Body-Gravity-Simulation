/** 
 * File: starGeneration.cpp
 * Description: Implementations for the StarGeneration class.
*/

#include "starGeneration.h"

StarGeneration::StarGeneration() {}

void StarGeneration::generateStarData(Parameters& parameters) {
    parameters.stars.clear();
    if (parameters.startingCondtion == 0) {
        generateUniformDistributionData(parameters);
    } else if (parameters.startingCondtion == 1) {
        generateElipitcalPlummerData(parameters);
    } else if (parameters.startingCondtion == 2) {
        magiLoadHdf5(parameters);
    }
}

void StarGeneration::generateUniformDistributionData(Parameters& parameters) {
    // Set scale parameters
    float galaxyUnitSize = parameters.genSizeKpc / 7.5;
    float normalizedMass = 1.0f;

    // Create RNG device set
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    if (parameters.secondaryStartingCondition == 0) {
        // Create a random distribution between [-1.0f, 1.0f]
        std::uniform_real_distribution<float> genSize(-galaxyUnitSize / 2, galaxyUnitSize / 2);

        // Generate n stars with random initial {x,y} positions between
        // [-1.0f, 1.0f], and random initial{x,y} velocty between [-0.1f, 0.1f]
        for (int i = 0; i < parameters.n; i++) {
            glm::vec3 position = glm::vec3(genSize(gen),  genSize(gen), (parameters.simulation3D) ? genSize(gen) : 0);
            glm::vec3 veloctiy = glm::vec3(0);
            glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
            glm::vec3 color = glm::vec3(0.0f,  0.0f,  0.0f);
            Body star(position, veloctiy, acceleration, normalizedMass/parameters.n, color);
            parameters.stars.push_back(star);
        }
    } else {
        // Create a random distribution for spherical coordinates
        std::uniform_real_distribution<float> genRadius(0.0f, galaxyUnitSize / 2);
        std::uniform_real_distribution<float> genTheta(0.0f, 2.0f * M_PI);
        std::uniform_real_distribution<float> genCosPhi(-1.0f, 1.0f);

        for (int i = 0; i < parameters.n; i++) {
            float radius = (parameters.simulation3D) ? std::cbrt(genRadius(gen)) : std::sqrt(genRadius(gen)); 
            float theta = genTheta(gen);
            float phi = (parameters.simulation3D) ? acos(genCosPhi(gen)) : 0.0f;
            float x = (parameters.simulation3D) ? radius * sin(phi) * cos(theta) : radius * cos(theta);
            float y = (parameters.simulation3D) ? radius * sin(phi) * sin(theta) : radius * sin(theta);
            float z = (parameters.simulation3D) ? radius * cos(phi) : 0;
            glm::vec3 position = glm::vec3(x, y, z);
            glm::vec3 veloctiy = glm::vec3(0);
            glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
            glm::vec3 color = glm::vec3(0.0f,  0.0f,  0.0f);
            Body star(position, veloctiy, acceleration, normalizedMass/parameters.n, color);
            parameters.stars.push_back(star);
        }
    }
}

void StarGeneration::generateElipitcalPlummerData(Parameters& parameters) {
    // Particle Parameters
    float galaxyUnitSize = parameters.genSizeKpc / 7.5;
    float normalizedMass = 1.0f;
    float scaleRadius = galaxyUnitSize / 2; 
    float radialClamp = 0.999f;
    float particleMass = normalizedMass/parameters.n;
    float gMax = 0.1f;
    float G_REAL = 4.5e-12f;
    float kpcPerUnit = parameters.genSizeKpc * 0.5f;
    float G = G_REAL * (parameters.billionSolarMass * 1e9f) / (kpcPerUnit * kpcPerUnit * kpcPerUnit);

    // Create RNG device set between [0.0f, 1.0f)
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_real_distribution<float> genRandom(0.0f, 1.0f);

    for (int i = 0; i < parameters.n; i++) {
        // Sample radius 
        float x1 = genRandom(gen) * radialClamp;
        float r = scaleRadius * pow(pow(x1, -2.0f / 3.0f) - 1.0f, -0.5f);

        // Sample position direction
        float x2 = genRandom(gen); 
        float x3 = genRandom(gen);
        float cosTheta = 1.0f - 2.0f * x2; 
        float sinTheta = sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta)); 
        float phi = 2.0f * M_PI * x3;
        glm::vec3 positionDirection = glm::vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
        glm::vec3 position = positionDirection * r;

        // Find escape velocity 
        float denominator = sqrt(r * r + scaleRadius * scaleRadius);
        float escapeVelocity = sqrt(2.0f * G * normalizedMass / denominator);

        // Sample speed Fraction 
        float q;
        while (true) {
            q = genRandom(gen);
            float g = q * q * pow(1.0f - q * q, 3.5f);

            float y = genRandom(gen) * gMax;
            if (y < g) {
                break;
            }
        }
        float speed = q * escapeVelocity;

        // Sample velocity direction 
        x2 = genRandom(gen); 
        x3 = genRandom(gen);
        cosTheta = 1.0f - 2.0f * x2; 
        sinTheta = sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta)); 
        phi = 2.0f * M_PI * x3;
        glm::vec3 velocityDirection = glm::vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
        glm::vec3 velocity = velocityDirection * speed;

        // Add Star
        glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
        glm::vec3 color = glm::vec3(0.0f,  0.0f,  0.0f);
        Body star(position, velocity, acceleration, particleMass, color);
        parameters.stars.push_back(star);
    }

    // Flatten Ellipse according to class
    int ellipseClass = parameters.secondaryStartingCondition;
    ellipseClass = std::clamp(ellipseClass, 0, 8);
    float axisRatio = 1.0f - (float)ellipseClass / 10.0f;
    glm::vec3 stretch = glm::vec3(1.0f, 1.0f, axisRatio);

    for (auto& star : parameters.stars) {
        glm::vec3 stretchedPosition = star.position * stretch;
        glm::vec3 stretchedVelocity = star.velocity * stretch;
        
        star.position = stretchedPosition;
        star.velocity = stretchedVelocity;
    }


    // Recenter
    glm::vec3 comPosition = glm::vec3(0.0f);
    glm::vec3 comVelocity = glm::vec3(0.0f);
    float massSum = 0.0f;

    for (auto& star : parameters.stars) {
        comPosition += star.position * star.mass;
        comVelocity += star.velocity * star.mass;
        massSum += star.mass;
    }
    comPosition /= massSum;
    comVelocity /= massSum;

    for (auto& star : parameters.stars) {
        star.position -= comPosition;
        star.velocity -= comVelocity;
    }
}

void StarGeneration::launchCustomGen(Parameters& parameters) {
    // Filename and Output directory
    outputFileName = parameters.generationHDF5FileName;
    std::string hdf5OutputDir = getHDF5OutputDir();

    // Default Parameter Options
    const float DISK_RADIAL_DISPERSION = -1.0f; 
    const float DISK_TOOMRE_Q          = 1.8f;
    const float DISK_RETROGRADE_FRAC   = 0.0f;

    std::string commandScript = "mkdir -p cfg/generated && mkdir -p " + hdf5OutputDir + " && ";

    int numberOfComponents = parameters.magiParameters.size();
    int generationN = 0;
    lastComponentOrder.clear();

    // Create Param Files in the MAGI directory for each component
    for (int i = 0; i < numberOfComponents; i++) {
        Parameters::MagiConfig& config = parameters.magiParameters[i]; 
        if (config.enabled) {
            lastComponentOrder.push_back(config.name);
            generationN += config.componentStarCount;
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
                if (config.magiProfileIndex == 1) { 
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
        Parameters::MagiConfig config = parameters.magiParameters[i]; 
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

    std::string logPath = hdf5OutputDir + "/" + outputFileName + ".log";

    //Construct WSL Linux Command
    commandScript += "bin/magi -config=generated/" + outputFileName + ".cfg -file=" + outputFileName +
               " -Ntot=" + std::to_string(generationN) +
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

    BOOL launched = CreateProcessA(
        nullptr, cmdBuffer.data(), nullptr, nullptr, FALSE,
        CREATE_NO_WINDOW, nullptr, nullptr, &si, &magiProcessInfo
    );
    if (!launched) {
        std::cout << "CreateProcessA failed, GetLastError=" << GetLastError() << std::endl;
    }
    processLaunched = launched;
}

std::string StarGeneration::getHDF5OutputDir() {
    // Get path to project directory
    std::string winPath = std::filesystem::current_path().string();
    std::string hdf5Path = winPath;
    if (hdf5Path.size() >= 2 && hdf5Path[1] == ':') {
        char driveLetter = std::tolower(static_cast<unsigned char>(hdf5Path[0]));
        hdf5Path = "/mnt/" + std::string(1, driveLetter) + hdf5Path.substr(2);
    }
    std::replace(hdf5Path.begin(), hdf5Path.end(), '\\', '/');

    // Add relative folder location from project root
    hdf5Path += "/magiGenerations/newGenerations";
    
    return hdf5Path;
}

bool StarGeneration::pollComplete() {
    if (!processLaunched) return false;
    DWORD exitCode;
    if (!GetExitCodeProcess(magiProcessInfo.hProcess, &exitCode)) {
        std::cout << "GetExitCodeProcess failed, GetLastError=" << GetLastError() << std::endl;
        return true;
    }
    if (exitCode != STILL_ACTIVE) {
        std::cout << "MAGI process exited with code " << exitCode << std::endl;
        CloseHandle(magiProcessInfo.hProcess);
        CloseHandle(magiProcessInfo.hThread);
        return true;
    }
    return false; 
}

void StarGeneration::annotateHdf5Components(const std::string& hdf5FilePath, Parameters& parameters) {
    try {
        H5::H5File file(hdf5FilePath, H5F_ACC_RDWR);

        // Open Header group
        H5::Group headerGroup;
        if (file.nameExists("Header")) {
            headerGroup = file.openGroup("Header");
        } else {
            headerGroup = file.createGroup("Header");
        }

        // Define dataspace
        const hsize_t dims[1] = {5};
        H5::DataSpace attrSpace(1, dims);
        std::vector<Parameters::MagiConfig> configs = parameters.magiParameters;
        int starCounts[5] = {(configs[0].enabled) ? configs[0].componentStarCount : 0, 
                             (configs[3].enabled) ? configs[3].componentStarCount : 0, 
                             (configs[4].enabled) ? configs[4].componentStarCount : 0, 
                             (configs[1].enabled) ? configs[1].componentStarCount : 0,
                             (configs[2].enabled) ? configs[2].componentStarCount : 0};

        const std::string attrName = "ComponentStarCounts";
        
        // Remove any existing attribute
        if (headerGroup.attrExists(attrName)) {
            headerGroup.removeAttr(attrName);
        }

        // Create and write the attribute
        H5::Attribute attr = headerGroup.createAttribute(
            attrName, H5::PredType::NATIVE_INT, attrSpace);
        attr.write(H5::PredType::NATIVE_INT, starCounts);

        attr.close();
        headerGroup.close();
        file.close();
    }
    catch (H5::Exception& e) {
        throw std::runtime_error("HDF5 error while annotating file: " + e.getDetailMsg());
    }  
}

void StarGeneration::magiLoadHdf5(Parameters& parameters) {
    // G value in kpc, solarmass, myr unit system
    const float G_REAL = 4.5e-12f; 

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
        std::string HDF5FileName = parameters.HDF5FileNames[parameters.selectedHDF5FileIndex];
        H5::H5File file("magiGenerations/newGenerations/" + HDF5FileName + ".hdf5", H5F_ACC_RDONLY);

        // Read ComponentStarCounts attribute 
        H5::Group headerGroup = file.openGroup("Header");
        H5::Attribute componentStarCountsAttr = headerGroup.openAttribute("ComponentStarCounts");

        int starCounts[5] = {0, 0, 0, 0, 0};
        componentStarCountsAttr.read(H5::PredType::NATIVE_INT, starCounts);

        componentStarCountsAttr.close();
        headerGroup.close();

        int currentStar = 0; 
        int currentComponent = 0;
        int componentChangeTarget = starCounts[0];

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
                while (currentStar == componentChangeTarget) {
                    currentComponent++;
                    componentChangeTarget += starCounts[currentComponent]; 
                }
                currentStar++;

                glm::vec3 position = glm::vec3(positionVector[3 * i], positionVector[3 * i + 1], positionVector[3 * i + 2]) * Lscale;
                glm::vec3 veloctiy = glm::vec3(velocityVector[3 * i], velocityVector[3 * i + 1], velocityVector[3 * i + 2]) * Vscale;
                glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
                glm::vec3 color = generateColor(currentComponent);
                Body star(position, veloctiy, acceleration, massVector[i], color);
                parameters.stars.push_back(star);
            }
        }
    } catch (H5::Exception& e) {
        std::cout << "HDF5 error: " << e.getCDetailMsg() << std::endl;
    }
}

glm::vec3 StarGeneration::generateColor(int componentType) {
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_real_distribution<float> genNormal(0.0f, 1.0f);
    float randomFloat = genNormal(gen);

    switch(componentType) { 
        // Dark Matter Halo
        case 0:
            colorRatios = {
            {glm::vec3(0.616f, 0.706f, 1.0f), 0.0f},        // Type O Star Color : Blue
            {glm::vec3(0.667f, 0.749f, 1.0f), 0.0f},        // Type B Star Color : Light Blue
            {glm::vec3(0.792f, 0.847f, 1.0f), 0.0f},        // Type A Star Color : White
            {glm::vec3(0.984f, 0.973f, 1.0f), 0.0f},        // Type F Star Color : Yellow White
            {glm::vec3(1.0f, 0.961f, 0.925f), 0.0f},        // Type G Star Color : Yellow
            {glm::vec3(1.0f, 0.824f, 0.631f), 0.0f},        // Type K Star Color : Orange
            {glm::vec3(1.0f, 0.745f, 0.498f), 0.0f},        // Type M Star Color : Red
            {glm::vec3(0.0f, 0.0f, 0.0f), 1.0f}};           // Invisible: Black
            break;
        
        // Thick Disk
        case 1:
            colorRatios = {
            {glm::vec3(0.616f, 0.706f, 1.0f), 0.0f},        // Type O Star Color : Blue
            {glm::vec3(0.667f, 0.749f, 1.0f), 0.0f},        // Type B Star Color : Light Blue
            {glm::vec3(0.792f, 0.847f, 1.0f), 0.01f},        // Type A Star Color : White
            {glm::vec3(0.984f, 0.973f, 1.0f), 0.01f},        // Type F Star Color : Yellow White
            {glm::vec3(1.0f, 0.961f, 0.925f), 0.2f},        // Type G Star Color : Yellow
            {glm::vec3(1.0f, 0.824f, 0.631f), 0.39f},        // Type K Star Color : Orange
            {glm::vec3(1.0f, 0.745f, 0.498f), 0.39f},        // Type M Star Color : Red
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f}};           // Invisible: Black         
            break;
        
         // Thin Disk
        case 2:
            colorRatios = {
            {glm::vec3(0.616f, 0.706f, 1.0f), 0.375f},        // Type O Star Color : Blue
            {glm::vec3(0.667f, 0.749f, 1.0f), 0.375f},        // Type B Star Color : Light Blue
            {glm::vec3(0.792f, 0.847f, 1.0f), 0.075f},        // Type A Star Color : White
            {glm::vec3(0.984f, 0.973f, 1.0f), 0.075f},        // Type F Star Color : Yellow White
            {glm::vec3(1.0f, 0.961f, 0.925f), 0.05f},        // Type G Star Color : Yellow
            {glm::vec3(1.0f, 0.824f, 0.631f), 0.025f},        // Type K Star Color : Orange
            {glm::vec3(1.0f, 0.745f, 0.498f), 0.025f},        // Type M Star Color : Red
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f}};           // Invisible: Black         
            break;
        
        // Stellar Halo
        case 3: 
            colorRatios = {
            {glm::vec3(0.616f, 0.706f, 1.0f), 0.0f},        // Type O Star Color : Blue
            {glm::vec3(0.667f, 0.749f, 1.0f), 0.0f},        // Type B Star Color : Light Blue
            {glm::vec3(0.792f, 0.847f, 1.0f), 0.005f},        // Type A Star Color : White
            {glm::vec3(0.984f, 0.973f, 1.0f), 0.005f},        // Type F Star Color : Yellow White
            {glm::vec3(1.0f, 0.961f, 0.925f), 0.15f},        // Type G Star Color : Yellow
            {glm::vec3(1.0f, 0.824f, 0.631f), 0.42f},        // Type K Star Color : Orange
            {glm::vec3(1.0f, 0.745f, 0.498f), 0.42f},        // Type M Star Color : Red
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f}};           // Invisible: Black        
            break; 
        
        // Bulge
        case 4:  
            colorRatios = {
            {glm::vec3(0.616f, 0.706f, 1.0f), 0.01f},        // Type O Star Color : Blue
            {glm::vec3(0.667f, 0.749f, 1.0f), 0.01f},        // Type B Star Color : Light Blue
            {glm::vec3(0.792f, 0.847f, 1.0f), 0.025f},        // Type A Star Color : White
            {glm::vec3(0.984f, 0.973f, 1.0f), 0.025f},        // Type F Star Color : Yellow White
            {glm::vec3(1.0f, 0.961f, 0.925f), 0.15f},        // Type G Star Color : Yellow
            {glm::vec3(1.0f, 0.824f, 0.631f), 0.39f},        // Type K Star Color : Orange
            {glm::vec3(1.0f, 0.745f, 0.498f), 0.39f},        // Type M Star Color : Red
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f}};           // Invisible: Black
            break;
        
        // Default 
        default:
            colorRatios = {
            {glm::vec3(0.616f, 0.706f, 1.0f), 0.0f},        // Type O Star Color : Blue
            {glm::vec3(0.667f, 0.749f, 1.0f), 0.0f},        // Type B Star Color : Light Blue
            {glm::vec3(0.792f, 0.847f, 1.0f), 1.0f},        // Type A Star Color : White
            {glm::vec3(0.984f, 0.973f, 1.0f), 0.0f},        // Type F Star Color : Yellow White
            {glm::vec3(1.0f, 0.961f, 0.925f), 0.0f},        // Type G Star Color : Yellow
            {glm::vec3(1.0f, 0.824f, 0.631f), 0.0f},        // Type K Star Color : Orange
            {glm::vec3(1.0f, 0.745f, 0.498f), 0.0f},        // Type M Star Color : Red
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f}};           // Invisible: Black
            break;
    }
    
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    float ratioSum = 0.0f;
    for (int i = 0; i < colorRatios.size(); i++) {
        ratioSum += colorRatios[i].second;
        if (ratioSum >= randomFloat) {
            color = colorRatios[i].first;
            break;
        }
    }

    // Reset colorRatios to Base
    
    return color;
}