/** 
 * File: shader.h
 * Description: Declarations for the Shader class. 
*/

#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader {
public: 
    // Shader Program ID
    unsigned int ID;

    /**
     * Shader Constructor: Creates a shader program from the 
     * file path of the given vertex shader and fragment shader 
     * that incorporates a vertex and fragment shader program. 
     */
    Shader(const char* vertexPath, const char* fragmentPath);

    /**
     * Shader Constructor: Creates shader program from the 
     * file path of the given compute shader that incorporates 
     * a compute shader program.  
     */
    Shader(const char* computePath);

    /**
     * Shader Constructor: Creates a shader program from the 
     * file path given by the compute shader  which attaches 
     * the declerations from the file given by the common file 
     * path onto the top of the compute shader. Intended for 
     * use where multiple compute shaders are used consecutively. 
     */
    Shader(const char* computePath, const char* commonPath, bool useCommon);

    /**
     * Active shader program.
     */
    void use();

    /**
     * Destructor.
    */
    void destroy();

};

#endif
