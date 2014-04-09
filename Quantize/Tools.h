//
//  Tools.h
//  Quantize
//
//  Created by Gerard Meier on 04/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include <OpenGL/gl3.h>


#include <fstream>
#include <stdarg.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include <cmath>
#include <string>
#include <deque>
#include <sys/time.h>  // For ::gettimeofday
#include "Math/Vector2.h"

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

static std::string StringReplace(const std::string& subject,
                                const std::string& search,
                                const std::string& replace)
{
    std::string result(subject);
    size_t pos = 0;
    
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        result.replace(pos, search.length(), replace);
        pos += std::max(replace.length(), search.length());
    }
    
    return result;
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
        //case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        //    Exit("Framebuffer Object Error: Dimensions do not match");
        //    break;
        //case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        //    Exit("Framebuffer Object Error: Formats");
        //    break;
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

    const std::string version("#version 410\n");

    if(FileExists(filename)) {
        
        
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
        
        
        
        std::function<std::string(std::string, int)> Parse;
        Parse = [&Parse] (const std::string& filename, int depth) -> std::string {
            std::string merged;
            
            if(depth > 10) {
                Exit("Aborting, too many nested includes. Is there a circular dependancy?");
            }
            
            if( ! FileExists(filename)) {
                Exit("Shader include file '%s' does not exist.", filename.c_str());
            }
            
            std::string source = ReadFile(filename);
    
            if(source.empty()) {
                Exit("File '%s' is empty.", filename.c_str());
            }

            
            for(std::string& line : StringExplode(source, "\n")) {
            
                if(StringStartsWith(line, "#include \"")) {
                    printf("-- %s\n", line.c_str());
                
                    std::string file = line.substr(10, line.length() - 11);
                
                    merged += Parse(file, depth + 1);
                } else {
                    merged += line + "\n";
                }
            }
            
            return merged;
        };

        std::string source = version + Parse(filename, 0);
        

        const char* csmells[] = {&source[0]};
        
        glShaderSource(shader, 1, csmells, NULL);
        GLError();

        glCompileShader(shader);
        GLError();
    
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0) {
            //printf("There were shader compile errors in:\n%s\n", source.c_str());
    
            auto chunks = StringExplode(source, "\n");
            for(int i = 0; i < chunks.size(); ++i) {
                printf("[%d] %s\n", i, chunks[i].c_str());
            }

            GLchar* log = new GLchar[logLength];
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            printf("Shader compile log:\n-------------\n%s-------------\n", log);
            delete[] log;
            
            exit(0);
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

    GLint logLength = 0;
    GLint status = GL_TRUE;
    
    glValidateProgram(program);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = new GLchar[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, log);
        printf("----------Program validate log----------\n%s----------------------------------------\n", log);
        delete[] log;
    }
    
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    
    
    if (status == GL_FALSE) {
        Exit("Much break. Such not valid. many kapot. Log size: %d", logLength);
    }
}

static double GetTiming()
{
    ::timeval now;
    int r = ::gettimeofday(&now, NULL);
    
    if(r != 0)
    {
        printf("GetMillies() Unable to retrieve system time, error code: '%i'.", r);
    }
    
    return now.tv_sec + (now.tv_usec / 1000000.0);
}

/// From: http://www.cplusplus.com/forum/general/1125/
static bool IsPrime (int num)
{
    if (num <=1)
        return false;
    else if (num == 2)         
        return true;
    else if (num % 2 == 0)
        return false;
    else
    {
        bool prime = true;
        int divisor = 3;
        double num_d = static_cast<double>(num);
        int upperLimit = static_cast<int>(sqrt(num_d) +1);
        
        while (divisor <= upperLimit)
        {
            if (num % divisor == 0)
                prime = false;
            divisor +=2;
        }
        return prime;
    }
}


/// Break a number into two factors, each < max. I'm pretty sure this
/// method will work most of the times.
static Furiosity::Vector2 Factors(int num, int max = 8192) {

    std::deque<int> primes;

    const int root = (int) std::sqrt(num);
    
    std::function<void(int)> recurse;

    // Mostly from: http://www.coderenaissance.com/2011/06/finding-prime-factors-in-javascript.html
    recurse = [&primes, &recurse, root] (int num) -> void {
        int x = 2;
        
        // if not divisible by 2
        if(num % x) {
             x = 3; // assign first odd
            
             // iterate odds
             while((num % x) && ((x = x + 2) < root)) {
                ; // nop
             }
        }
        
        //if no factor found then num is prime
        x = (x <= root) ? x : num;
        
        if(x != num && num > 0) {
            recurse(num / x);
        }
       
        primes.push_back(x);//push latest prime factor
    };

    recurse(num);

    int x = 1;
    int y = 1;
    
    // Sorting increases the odds of this thing actually working
    std::sort(primes.begin(), primes.end());
    
    //for(int p : primes) {
    //    printf("%d\n", p);
    //}
    
    // Grow X until the upper limit is reached.
    while( ! primes.empty() && x * primes.front() < max) {
        x *= primes.front();
        primes.pop_front();
    }
    
    // Pass the remaining primes to y.
    while( ! primes.empty() ) {
        y *= primes.front();
        primes.pop_front();
    }

    //if(x > 16384 || y > 16384) {
    //    Exit("Primes don't work. Add padding bytes or more photons. Result: %d x %d", x, y);
    //}

    return Furiosity::Vector2(x, y);
}

