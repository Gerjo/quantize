//
//  Textures.h
//  Quantize
//
//  Created by Gerard Meier on 05/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include "Tools.h"

#include "libPNG/png.h"

struct Textures {
    static GLuint LoadPNG(const std::string& filename) {
    
        if( ! FileExists(filename)) {
            Exit("Cannot open png: %s.", filename.c_str());
        }
    
        FILE* fp = ::fopen(filename.c_str(), "r");

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop info_ptr  = png_create_info_struct(png_ptr);

        if (::setjmp(png_jmpbuf(png_ptr))) {
            Exit("liPNG had some error.");
        }
        
        png_init_io(png_ptr, fp);
        
        unsigned int sig_read = 0;
        png_set_sig_bytes(png_ptr, sig_read);
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
        
        
        png_uint_32 width, height; int bit_depth, color_type, interlace_type;
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
      
        unsigned long row_bytes = png_get_rowbytes(png_ptr, info_ptr);
        unsigned char* data = (unsigned char*) ::malloc(row_bytes * height);

        png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    
        // Align bytes as OpenGL expects them.
        for (unsigned int i = 0; i < height; i++) {
            memcpy(data+(row_bytes * i), row_pointers[i], row_bytes);
        }
        
        // Clean up after the read, and free any memory allocated
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL /* png_infopp_NULL */);
        
        // Close the file
        ::fclose(fp);
    
        
        GLint internalFormat;
        
        // Available PNG formats:
        switch (color_type) {
            case PNG_COLOR_TYPE_RGBA:
                internalFormat = GL_RGBA;
                break;
                
            case PNG_COLOR_TYPE_RGB:
                internalFormat = GL_RGB;
                break;
            case PNG_COLOR_TYPE_GRAY:
                internalFormat = GL_LUMINANCE;
                break;
                
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                internalFormat = GL_LUMINANCE_ALPHA;
                break;
                
            default:
                // This probably means the switch case needs an addition.
                Exit("unsupported PNG color type: [%d].", color_type);
        }
        
        // Find a free texture index or "name".
        GLuint texture;
        glGenTextures(1, &texture);
        GLError();

        // Find the index to global state
        glBindTexture(GL_TEXTURE_2D, texture);
        GLError();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        
        printf("Uploadeding %lu bytes to the GPU...", row_bytes * height);
        
        glTexImage2D(GL_TEXTURE_2D,                 // What (target)
                 0,                                 // Mip-map level
                 internalFormat,                    // Internal format
                 width,                             // Width
                 height,                            // Height
                 0,                                 // Border
                 internalFormat,                    // Format (how to use)
                 GL_UNSIGNED_BYTE,                  // Type   (how to intepret)
                 data);                             // Data
        GLError();
        
        printf(" done.\n");
        
        // Main reason why we use PNG. Random models from the internet come with
        // random formats, and generating mipmaps may be non-trivial. This will
        // always work :)
        glGenerateMipmap(GL_TEXTURE_2D);
        
        GLenum error = glGetError();
    
        if (error != GL_NO_ERROR) {
            Exit("Error uploading PNG texture %s to GPU. glError: 0x%04X\n", filename.c_str(), error);
        }
        
        // Data is on the GPU's RAM, release it from the CPU's RAM.
        ::free(data);
    
        return texture;
    }
};