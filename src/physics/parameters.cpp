/** 
 * File: parameters.cpp
 * Description: Implementations for the Parameters class. 
*/

#include "parameters.h"

Parameters::MagiConfig::MagiConfig(std::string name, int category) {
    this->name = name;
    this->category = category;
}

void Parameters::MagiConfig::resetConfig() {
    magiProfileIndex = 0;
    componentStarCount = 1; 
    componentMass = 1.0f; 
    scaleRadius = 1.0f; 
    scaleHeight = 1.0f; 
    extraParam = 1.0f;
}


Parameters::Parameters() {
    Parameters::MagiConfig darkMatterConfig("Dark Matter Halo", 0);
    this->magiParameters.push_back(darkMatterConfig);

    Parameters::MagiConfig stellarConfig("Stellar Halo", 0);
    this->magiParameters.push_back(stellarConfig);

    Parameters::MagiConfig bulgeConfig("Bulge", 0);
    this->magiParameters.push_back(bulgeConfig);

    Parameters::MagiConfig thickDiskConfig("Thick Disk", 1);
    this->magiParameters.push_back(thickDiskConfig);

    Parameters::MagiConfig thinDiskConfig("Thin Disk", 1);
    this->magiParameters.push_back(thinDiskConfig);
}

void Parameters::updateHDF5FileNames() {
    // Set Sarch Folder Path
    const std::string folderPath = "magiGenerations/newGenerations";

    hdf5FileNameStrings.clear();
    HDF5FileNames.clear();

    // Check that Folder exists
    if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
        return;
    }

    // Add all .hdf5 file names
    for (const auto& fileInFolder : std::filesystem::directory_iterator(folderPath)) {
        if (fileInFolder.is_regular_file() && fileInFolder.path().extension() == ".hdf5") {
            hdf5FileNameStrings.push_back(fileInFolder.path().stem().string());
        }
    }
    
    HDF5FileNames.reserve(hdf5FileNameStrings.size());
    for (const auto& name : hdf5FileNameStrings) {
        HDF5FileNames.push_back(name.c_str());
    }

    if (selectedHDF5FileIndex >= HDF5FileNames.size()) {
        selectedHDF5FileIndex = 0;
    }
}
