//
//  Textures.cpp
//  Quantize
//
//  Created by Gerard Meier on 19/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#include "Textures.h"


// Allocate memory for statics.
std::map<const std::string, std::weak_ptr<GLuint>> Textures::cache;
std::vector<GLuint> Textures::samplers;
