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
     * Active shader program.
     */
    void use();

    /**
     * Set uniform variables for 4Vectors with type float, boolean or int. 
     */
    void setVec4f(const std::string &name, float x, float y, float z, float w) const;
    void setVec4b(const std::string &name, bool x, bool y, bool z, bool w) const;
    void setVec4i(const std::string &name, int x, int y, int z, int w) const;

    /**
     * Destructor.
    */
    void destroy();

};

#endif
