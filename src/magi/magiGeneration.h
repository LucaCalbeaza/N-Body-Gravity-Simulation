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

class MagiGeneration {
public:
    
    /**
     *  
     */
    MagiGeneration(std::vector<Body>& stars, GUI::inputParameters parameters);

    /**
     * 
     */
    void magiCustomGeneration();
    
    /**
     * 
     */
    std::vector<Body> magiLoadHdf5(GUI::inputParameters parameters);
};

#endif
