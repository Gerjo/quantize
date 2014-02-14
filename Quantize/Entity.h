//
//  Entity.h
//  Quantize
//
//  Created by Gerard Meier on 14/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include <vector>
#include <memory>

#include "Math/Matrix44.h"

using namespace Furiosity;

class Quantize;

class Entity {
public:
   
    /// Models nested within this model. Transforms are inherited, this is used
    /// for Collada's nested nodes.
    std::vector<std::shared_ptr<Entity>> sub;

    Matrix44 transform;

    Entity() {
    
    }
    
    Entity(std::shared_ptr<Entity> e) {
        sub.push_back(e);
    }
    
    virtual void update(Quantize* q, const Matrix44& parent, const float dt) {
        for(auto model : sub) {
            model.get()->update(q, parent * transform, dt);
        }
    }
    
    
    std::shared_ptr<Entity> getByName(const std::string& name) {
        /*
        std::shared_ptr<Model>
        
        for(Model* model : sub) {
            if(model->name == name) {
                // found!
                return model;
            } else {
            
                // Recurse into tree
                r = model->getByName(name);
                
                // It's found, unwind recursion.
                if(r != nullptr) {
                    break;
                }
                
            }
        }
        
        return r;*/
        
        return std::shared_ptr<Entity>();
    }
};
