//
//  Collada.h
//  Quantize
//
//  Created by Gerard Meier on 10/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once

#include "tinyXML/Xml.h"
#include <map>
#include <vector>
#include <istream>
#include <sstream>
#include <memory>

#include "Model.h"
#include "Tools.h"
#include "Textures.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

using std::string;
using namespace tinyxml2;
using namespace Furiosity;

struct Collada {

    /// Helper struct to store some grouped data.
    struct Effect {
        Vector4 diffuse;
        Vector4 specular;
        float shininess;
        unsigned texture;
        
        std::string surface;
        std::string sampler2D;
        
        Effect() : texture(0), shininess(1) {
        
        }
    };

    // A typedef to make the situation more obvious.
    using Material = Effect;
    
    static std::shared_ptr<Model> FromFile(const std::string& filename) {
    
        static std::map<string, std::weak_ptr<Model>> cache;
        
        if(cache.find(filename) != cache.end()) {
            if( ! cache[filename].expired()) {
                printf("Model vbo loaded from cache.\n");
                // Promote the weak reference to a shared reference and return it.
                return cache[filename].lock();
            }
        }
        
        if( ! FileExists(filename)) {
            Exit("File does not exist: %s.", filename.c_str());
        }
        
        std::string data = ReadFile(filename);
        
        if(data.empty()) {
            Exit("File is empty: %s.", filename.c_str());
        }
    
        std::map<string, string> imageFiles;
        
        std::map<string, std::vector<std::vector<float> > > floatSources;
        
        std::map<string, std::vector<VertexData> > vertexData;
        
        std::map<string, Effect> effects;
        std::map<string, Material> materials;

        
        //std::vector<Model*> models;
        Model* root = new Model();
    
        XMLDocument doc;
        doc.Parse(data.c_str());
        
        
        FOREACH_XML_ELEMENT(element, doc.FirstChildElement()) {
            string category(element->Value());
            
            printf("Parsing Category: %s\n", element->Value());

            if(category == "library_images") {
                FOREACH_XML_ELEMENT(key, element) {
                    string type(key->Value());
                    
                    if(type == "image") {
                        string id(string("#") + key->Attribute("id"));
                        
                        string file(key->FirstChildElement()->GetText());
                    
                        imageFiles.insert(make_pair(id, file));
                        
                        printf("  %s -> %s\n", id.c_str(), file.c_str());
                    }
                }
                
            } else if(category == "library_effects") {
                FOREACH_XML_ELEMENT(effect, element) {
                    string effectid(string("#") + effect->Attribute("id"));
                    
                    Effect e;
                    
                    // Iterate over the first child, we assume its the COMMON profile.
                    FOREACH_XML_ELEMENT(property, effect->FirstChildElement()) {
                    
                        if(strcmp("newparam", property->Name()) == 0) {
                            //printf("param: %s\n",  property->FirstChildElement()->Name());
                        
                            if(strcmp("surface", property->FirstChildElement()->Name()) == 0) {
                                e.surface = string("#") + property->FirstChildElement()->FirstChildElement()->GetText();
                                
                            } else if(strcmp("sampler2D", property->FirstChildElement()->Name()) == 0) {
                                e.sampler2D = string("#") + property->FirstChildElement()->FirstChildElement()->GetText();
                            }
                        } else if(strcmp("technique", property->Name()) == 0) {
                            // Take note of diffuse, specular and shinyness
                        }
                    }
                    
                    effects.insert(make_pair(
                        effectid,
                        e
                    ));
                    
                    printf("  Making effect[%s] with texture [%s]\n", effectid.c_str(), e.surface.c_str());
                }
            
            
            } else if(category == "library_materials") {
                FOREACH_XML_ELEMENT(material, element) {
                    if(strcmp("material", material->Name()) == 0) {
                        string materialid(string("#") + material->Attribute("id"));
                        string source(material->FirstChildElement()->Attribute("url"));
                        
                        if(effects.find(source) == effects.end()) {
                            Exit("Material[%s] requested effect[%s] but it was not found.", materialid.c_str(), source.c_str());
                        }
                        
                        // We copy the effect directly. Eventually we should parse this node
                        // to determine if the material overrides certain effect properties.
                        materials.insert(make_pair(
                            materialid,
                            effects[source]
                        ));
                        
                        printf("  Making material[%s] using effect[%s]\n", materialid.c_str(), source.c_str());

                    } else {
                        Exit("Non material? %s", material->Name());
                    }
                }
            } else if(category == "library_geometries") {
                FOREACH_XML_ELEMENT(geometry, element) {
                    string type(geometry->Value());
                    
                    if(type == "geometry") {
                        string geometryid(string("#") + geometry->Attribute("id"));
                        
                        printf("  Parsing geometry: %s\n", geometryid.c_str());

                        FOREACH_XML_ELEMENT(mesh, geometry) {
                        
                            if(strcmp("mesh", mesh->Value()) == 0) {
                            
                                FOREACH_XML_ELEMENT(source, mesh) {
                                    
                                    string type(source->Value());
                                    
                                    if(type == "source") {
                                        //string sourceid(string("#") + source->FirstChildElement()->Attribute("id"));
                                        string sourceid(string("#") + source->Attribute("id"));
                                   
                                        string floats;
                                        int stride = 0;
                                        int count  = source->FirstChildElement()->IntAttribute("count");
                                        
                                        
                                        // Find the floating points and their stride.
                                        FOREACH_XML_ELEMENT(property, source) {
                                            string propName(property->Value());
                                            
                                            if(propName == "float_array") {
                                                floats = property->GetText();
                                            } else if(propName == "technique_common") {
                                                stride = property->FirstChildElement()->IntAttribute("stride");
                                            }
                                            //printf("        %s\n", propName.c_str());
                                        }
                                        
                                        if(stride <= 0) {
                                            Exit("Unknown stride: %d", stride);
                                        }
                                        
                                        // Use a stream to read the floats from.
                                        std::stringstream stream(floats);
                                        
                                        // Preallocate space in the vector
                                        std::vector<std::vector<float> > vertices(int(count / stride));
                                        vertices.clear();
                                        
                                        // For all floats
                                        for(int i = 0; i < count; ) {
                                            vertices.emplace_back();
                                        
                                            // Read n floats into a vector
                                            for(int j = 0; j < stride; ++j, ++i) {
                                                float f;
                                                stream >> f;
                                                
                                                vertices.back().push_back(f);
                                            }
                                        }
                                        
                                        printf("    Storing float array[%lu] with id %s\n", vertices.size(), sourceid.c_str());

                                        
                                        // Create a lookup table entry
                                        floatSources.insert(make_pair(
                                            sourceid,
                                            vertices//std::move(vertices)
                                        ));
                                        
                                        
                                    } else if(type == "vertices") {
                                        string alias(string("#") + source->Attribute("id"));
                                        
                                        string original = source->FirstChildElement()->Attribute("source");
                                        
                                        if(floatSources.find(original) == floatSources.end()) {
                                            Exit("Requested source %s for alias %s not found.", original.c_str(), alias.c_str());
                                        }
                                        
                                        // Make a copy for now.
                                        floatSources.insert(make_pair(
                                            alias,
                                            floatSources[original]
                                        ));
                                        
                                        printf("    %s uses alias %s\n", original.c_str(), alias.c_str());
                                        
                                    } else if(type == "polylist") {
                                        int count = source->IntAttribute("count");
                                        string material = source->Attribute("material");
                                        
                                        printf("    Creating a polylist with %d entries [%s] \n", count, material.c_str());

                                    
                                        // offset to sourceid mapping
                                        std::vector<string> inputs(3, "");
                                        std::vector<string> semantics(3, "");
                                        std::vector<int> vcount;//(count, 0); vcount.clear();
                                        std::vector<int> indices;//(count, 0); indices.clear();
                                    
                                        FOREACH_XML_ELEMENT(attribute, source) {
                                            
                                            if(strcmp("input", attribute->Name()) == 0) {
                                                int offset(attribute->IntAttribute("offset"));
                                                string semantic(attribute->Attribute("semantic"));
                                                string source(attribute->Attribute("source"));
                                                
                                                printf("      Offset %d with id %s [%s]\n", offset, source.c_str(), semantic.c_str());
                                                inputs[offset]    = source;
                                                semantics[offset] = semantic;
                                                if(offset > 3 || offset < 0) {
                                                    Exit("Invalid offset: %d. Not in [0,1,2]", offset);
                                                }
                                                
                                            } else if(strcmp("vcount", attribute->Name()) == 0) {
                                                std::stringstream stream(attribute->GetText());
                                                
                                                for(int d; stream >> d;) {
                                                
                                                    if(d != 3) {
                                                        Exit("Face does not consist of a triangle. Found: %d vertices.", d);
                                                    }
                                                
                                                    vcount.push_back(d);
                                                }
                                            } else if(strcmp("p", attribute->Name()) == 0) {
                                                std::stringstream stream(attribute->GetText());
                                              
                                                for(int d; stream >> d;) {
                                                    indices.push_back(d);
                                                }
                                                
                                                printf("      Read %lu indices\n", indices.size());
                                            }
                                        }
                                    
                                        //printf("%lu indices, %lu vcount\n", indices.size(), vcount.size());
                                    
                                        //for(const int v : vcount) {
                                            
                                        //}
                                        
                                        std::vector<VertexData> vertices;
                                        
                                        int actualCount = (int)indices.size();
                                        
                                        //if(actualCount / 3 != count) {
                                        //    Exit("wut? %d %d %d", actualCount, count, actualCount / 3);
                                        //}
                                    
                                        for(int i = 0; i < actualCount; ) {
                                            VertexData data;
                                            
                                            bool addedVertex = false;
                                            bool addedUV     = false;
                                            bool addedNormal = false;
                                            
                                            for(int j = 0; j < inputs.size() && i < actualCount; ++j, ++i) {
                                                string sourceid = inputs[j];
                                                
                                                if(inputs[j].empty()) {
                                                    //printf("      Skipping index %d, it's empty.\n", j);
                                                    
                                                    // There's probably no UV coordinate.
                                                    --i;
                                                    continue;
                                                }
                                                
                                                if(floatSources.find(sourceid) == floatSources.end()) {
                                                    Exit("polylist requested source id [%s] but was not found.", sourceid.c_str());
                                                }
                                                
                                                auto& lookup = floatSources[sourceid];
                                                
                                                if(lookup.size() < indices[i] && indices[i] >= 0) {
                                                    Exit("Requested index %d of %lu for source id %s is out-of-bounds", indices[i], lookup.size(), sourceid.c_str());
                                                }
                                                
                                                auto& entry = lookup[indices[i]];
                                                
                                                if(entry.empty()) {
                                                    Exit("The entry is empty, expected 2 or 3 floats. Requested entry index: %d", indices[i]);
                                                }
                                                
                                                // Map them to the format OpenGL expects. I'm assuming these
                                                // so-called semantic names are constant accros all collada
                                                // model files
                                                if(semantics[j] == "VERTEX") {
                                                    data.position.x = entry[0];
                                                    data.position.y = entry[1];
                                                    data.position.z = entry[2];
                                                    addedVertex = true;
                                                } else if(semantics[j] == "NORMAL") {
                                                    data.normal.x = entry[0];
                                                    data.normal.y = entry[1];
                                                    data.normal.z = entry[2];
                                                    addedNormal = true;
                                                } else if(semantics[j] == "TEXCOORD") {
                                                    data.uv.x = entry[0];
                                                    
                                                    // Unsure how this "flipping" works with repeated textures.
                                                    data.uv.y = 1-entry[1];
                                                    addedUV = true;
                                                }
                                            }
                                            
                                            if(!addedVertex || !addedNormal) {
                                                Exit("Not all 2 components are added. vertex: %d, normal: %d.", (int)addedVertex, (int)addedNormal);
                                            }
                                            
                                            vertices.push_back(data);
                                        }
                                        
                                        printf("   Adding vertexData[%lu] with id %s\n", vertices.size(), geometryid.c_str());
                                        
                                        vertexData.insert(make_pair(
                                            geometryid,
                                            vertices
                                        ));
                                    }
                                }
                            }
                        }
                    }
                }
                
            } else if(category == "library_visual_scenes") {
                
                FOREACH_XML_ELEMENT(scene, element) {
                    string type(scene->Value());
                
                    printf("  %s\n", type.c_str());

                
                    FOREACH_XML_ELEMENT(node, scene) {
                        string nodeid(string("#") + node->Attribute("id"));
                        string name(node->Attribute("name"));
                        
                        printf("    node with id: %s and name %s \n", nodeid.c_str(), name.c_str());
                        
                        Model* model = new Model();
                        
                        model->name = name;
                        model->group = name;
                        
                        Vector3 translate(0, 0, 0);
                        Vector3 scale(1, 1, 1);
                        
                        std::vector<std::pair<Vector3, float> > rotate;
                        
                        FOREACH_XML_ELEMENT(property, node) {
                            string propName(property->Value());
                            
                            //printf("      %s\n", property->Value());
                            
                            if(propName == "translate") {
                                auto chunks = StringExplode(property->GetText(), " ");
                            
                                translate.x = chunks.size() > 0 ? atof(chunks[0].c_str()) : 0;
                                translate.y = chunks.size() > 1 ? atof(chunks[1].c_str()) : 0;
                                translate.z = chunks.size() > 2 ? atof(chunks[2].c_str()) : 0;
                                
                            } else if(propName == "rotate") {
                                auto chunks = StringExplode(property->GetText(), " ");
                                
                                if(chunks.size() != 4) {
                                    Exit("Only angle axis rotation is supported. Found %lu/4 numbers.", chunks.size());
                                }
                                
                                Vector3 axis(
                                    atof(chunks[0].c_str()),
                                    atof(chunks[1].c_str()),
                                    atof(chunks[2].c_str())
                                );
                                
                                // Degrees to radians
                                float angle = atof(chunks[3].c_str()) / (3.14159268f/180.0f);

                                if(angle != 0) {
                                    rotate.push_back(std::make_pair(
                                        axis,
                                        angle
                                    ));
                                }
                                
                            } else if(propName == "scale") {
                                auto chunks = StringExplode(property->GetText(), " ");
                                
                                scale.x = chunks.size() > 0 ? atof(chunks[0].c_str()) : 1;
                                scale.y = chunks.size() > 1 ? atof(chunks[1].c_str()) : 1;
                                scale.z = chunks.size() > 2 ? atof(chunks[2].c_str()) : 1;
                            
                            } else if(propName == "instance_geometry") {
                                string url(property->Attribute("url"));
                                
                                
                                if(vertexData.find(url) == vertexData.end()) {
                                    Exit("Requested vertex data %s not found.", url.c_str());
                                }
                                
                                auto& vertices = vertexData.at(url);
                                
                                // Taking a leap of faith here. Suddenly jumping 3 layers without error checking.
                                string material = property->FirstChildElement()->FirstChildElement()->FirstChildElement()->Attribute("target");
                                
                                if(materials.find(material) == materials.end()) {
                                    Exit("Node[%s] requested material[%s], but it was not found.", nodeid.c_str(), material.c_str());
                                }
                                
                                Material m = materials[material];
                                
                                // To keep the shader and other logic simple, we
                                // load a transparent texture if no texture is used.
                                string textureFile = "models/transparent.png";
                                
                                if(imageFiles.find(m.surface) != imageFiles.end()) {
                                    textureFile = imageFiles[m.surface];
                                    //Exit("Node[%s] requested material[%s] which requested texture[%s] which is not found.",  nodeid.c_str(), material.c_str(), m.surface.c_str());
                                }
                                
                                printf("      ");
                                model->texture = Textures::LoadPNG("models/Plane/Soil.png");
                                
                                
                                printf("      using geometry[%s] and %lu vertices with material[%s].\n", url.c_str(), vertices.size(), material.c_str());
                                
                                model->vertices = vertices;
                                
                                // Lookup the texture sampler index.
                                const GLuint sampler = (GLuint) std::distance(
                                    Textures::samplers.begin(),
                                    std::find(
                                        Textures::samplers.begin(),
                                        Textures::samplers.end(),
                                        * model->texture)
                                    );
                                
                                printf("------\nSampler: %u\n", sampler);
                                
                                // Generate some indices
                                for(size_t i = 0; i < vertices.size(); ++i) {
                                    model->indices.push_back((int) i);
                                    
                                    model->vertices[i].sampler = sampler;
                                    
                                }
                                
                                root->sub.push_back(std::shared_ptr<Model>(model));
                            }
                            
                            // TODO: might have to change the order of multiplication.
                            Matrix44 transform;
                            
                            
                            transform.SetIndentity();
                            
                            transform = transform * Matrix44::CreateScale(scale);
                            
                            for(auto& r : rotate) {
                                transform = transform * Matrix44::CreateRotate(r.second, r.first.x, r.first.y, r.first.y);
                            }
                            
                            transform = transform * Matrix44::CreateTranslation(translate.x, translate.y, translate.z);
                            
                            model->transform = transform;
                            model->upload();
                            
                        }
                    }
                }
            } else {
                // Not supported yet.
            }
        }
    
    
        // Create smart pointer to handle life-cycle, a custom deleter is provided.
        auto ptr = std::shared_ptr<Model>(root, [] (Model* model) {
            // Remove from RAM.
            delete model;
        });
        
        cache.insert(make_pair(filename, ptr));
        
        return ptr;
    }

};