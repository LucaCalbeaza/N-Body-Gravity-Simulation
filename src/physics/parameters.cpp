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
