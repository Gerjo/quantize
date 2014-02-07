//
//  Tools.h
//  Quantize
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


#include <fstream>
#include <stdarg.h>
#include <cstdio>
#include <cassert>
#include <vector>

static std::string ReadFile(const std::string& filename) {
    std::ifstream t(filename.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    
    return str;
}


static bool FileExists(const std::string& filename) {
    std::ifstream s(filename.c_str());
    return s.is_open();
}

// TODO: get rid of this, after development.
static void Exit(std::string format, ...) {
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stdout, ("Exiting with error: " + format + "\n").c_str(), argptr);
    va_end(argptr);
    
    assert(0);
    exit(1);
}



static bool StringEndsWith(const std::string& subject,
                               const std::string& suffix)
{
    // Early out test:
    if(suffix.length() > subject.length())
    {
        return false;
    }
    
    // Resort to difficult to read C++ logic:
    return subject.compare(
        subject.length() - suffix.length(),
        suffix.length(),
        suffix
    ) == 0;
}

static bool StringStartsWith(const std::string& subject,
                                 const std::string& prefix)
{
    // Early out, prefix is longer than the subject:
    if(prefix.length() > subject.length())
    {
        return false;
    }
    
    // Compare per character:
    for(int i = 0; i < prefix.length(); ++i)
    {
        if(subject[i] != prefix[i])
        {
            return false;
        }
    }
    
    return true;
}

static std::vector<std::string> StringExplode(const std::string& subject,
                                                  const std::string& delimiter)
{
    std::vector<std::string> result;
    
    size_t offset = 0;
    
    for(size_t at = 0; (at = subject.find_first_of(delimiter, offset)) != std::string::npos; )
    {
        const std::string str = subject.substr(offset, at - offset);

        result.push_back(str);
        
        offset += str.length() + delimiter.length();
    }
    
    // There may be remnants:
    if(offset < subject.length())
    {
        result.push_back(subject.substr(offset));
    }
    
    return result;
}

/// TODO: macro with __line__ etc.
static void GLError() {
    
    // Error lookup
    auto toString = [] (const GLenum error) {
        switch( error ) {
            case GL_INVALID_ENUM:
                return "GL_INVALID_ENUM";
                
            case GL_INVALID_VALUE:
                return "GL_INVALID_VALUE";
                
            case GL_INVALID_OPERATION:
                return "GL_INVALID_OPERATION";
                
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return "GL_INVALID_FRAMEBUFFER_OPERATION";
                
            case GL_OUT_OF_MEMORY:
                return "GL_OUT_OF_MEMORY";
        }
        return "UNKNOWN";
    };
    
    bool hasError = false;
    
    for(GLenum error; (error = glGetError()) != GL_NO_ERROR; ) {
        printf("GLError: %s\n", toString(error));
        hasError = true;
    }
    
    if(hasError) {
        Exit("There were errors.");
    }
}

static void GLFBError() {
    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    switch (framebufferStatus) {
        case GL_FRAMEBUFFER_COMPLETE: break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            Exit("Framebuffer Object Error: Attachment Point Unconnected");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            Exit("Framebuffer Object Error: Missing Attachment");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            Exit("Framebuffer Object Error: Dimensions do not match");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            Exit("Framebuffer Object Error: Formats");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            Exit("Framebuffer Object Error: Draw Buffer");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            Exit("Framebuffer Object Error: Read Buffer");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            Exit("Framebuffer Object Error: Unsupported Framebuffer Configuration");
            break;
        default:
            Exit("Framebuffer Object Error: Unkown Framebuffer Object Failure");
            break;
    }
}

static GLuint CompileShader(const std::string& filename) {
    if(FileExists(filename)) {
        std::string source = ReadFile(filename);
    
        if(source.empty()) {
            Exit("File %f is empty.", filename.c_str());
        }
        
        GLenum type = 0;
        
        // Deduct the shader type from the filename.
        if(StringEndsWith(filename, ".vsh")) {
            type = GL_VERTEX_SHADER;
        } else if(StringEndsWith(filename, ".fsh")) {
            type = GL_FRAGMENT_SHADER;
        } else {
            Exit("Cannot deduct shader type. Files must end with .vsh or .fsh.");
        }
        
        GLuint shader = glCreateShader(type);
        GLError();

        const char* csmells[] = {source.c_str()};
        
        glShaderSource(shader, 1, csmells, NULL);
        GLError();

        glCompileShader(shader);
        GLError();
    
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0) {
            GLchar* log = new GLchar[logLength];
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            printf("Shader compile log:\n%s\n", log);
            delete[] log;
            
            Exit("There were shader compile errors in %s.", source.c_str());
        }
        
        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == 0) {
            glDeleteShader(shader);
        }

        
        return shader;
        
    } else {
        Exit("Cannot read %s.", filename.c_str());
    }
    
    return 0;
}


static void GLValidateProgram(const GLuint program) {

    GLint logLength, status;
    
    glValidateProgram(program);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = new GLchar[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, log);
        printf("Program validate log:\n%s\n", log);
        delete[] log;
    }
    
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    
    
    if (status == 0) {
        Exit("Much break. Such not valid. many kapot.");
    }
}
