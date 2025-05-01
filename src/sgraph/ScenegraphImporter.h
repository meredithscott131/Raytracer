#ifndef _SCENEGRAPHIMPORTER_H_
#define _SCENEGRAPHIMPORTER_H_

#include "IScenegraph.h"
#include "Scenegraph.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "PolygonMesh.h"
#include "Material.h"
#include <istream>
#include <map>
#include <string>
#include <iostream>
#include <math.h>

#include <ImageLoader.h>
#include <PPMImageLoader.h>
#include "TextureImage.h"
using namespace std;

namespace sgraph {
    class ScenegraphImporter {
        public:
            ScenegraphImporter() {
                root = NULL;
                ImageLoader *loader = new PPMImageLoader();
                loader->load("textures/white.ppm");
                util::TextureImage *texture = new util::TextureImage(loader->getPixels(), loader->getWidth(), loader->getHeight(), "default");
                textureObjects["default"] = texture;
            }

            IScenegraph *parse(istream& input) {
                string command;
                string inputWithOutCommentsString = stripComments(input);
                istringstream inputWithOutComments(inputWithOutCommentsString);
                while (inputWithOutComments >> command) {
                    //cout << "Read " << command << endl;
                    if (command == "instance") {
                        string name,path;
                        inputWithOutComments >> name >> path;
                        //cout << "Read " << name << " " << path << endl;
                        meshPaths[name] = path;
                        ifstream in(path);
                       if (in.is_open()) {
                        util::PolygonMesh<VertexAttrib> mesh = util::ObjImporter<VertexAttrib>::importFile(in,false);
                        meshes[name] = mesh;
                       } 
                    }
                    else if (command == "group") {
                        parseGroup(inputWithOutComments);
                    }
                    else if (command == "leaf") {
                        parseLeaf(inputWithOutComments);

                    }
                    else if (command == "material") {
                        parseMaterial(inputWithOutComments);
                    }
                    else if (command == "scale") {
                        parseScale(inputWithOutComments);
                    }
                    else if (command == "rotate") {
                        parseRotate(inputWithOutComments);
                    }
                    else if (command == "translate") {
                        parseTranslate(inputWithOutComments);
                    }
                    else if (command == "copy") {
                        parseCopy(inputWithOutComments);
                    }
                    else if (command == "import") {
                        parseImport(inputWithOutComments);
                    }
                    else if (command == "assign-material") {
                        parseAssignMaterial(inputWithOutComments);
                    }
                    else if (command == "add-child") {
                        parseAddChild(inputWithOutComments);
                    }
                    else if (command == "assign-root") {
                        parseSetRoot(inputWithOutComments);
                    }
                    else if (command == "texture") {
                        parseTexture(inputWithOutComments);
                    }
                    else if (command == "assign-texture") {
                        parseAssignTexture(inputWithOutComments);
                    }
                    else {
                        throw runtime_error("Unrecognized or out-of-place command: "+command);
                    }
                }
                if (root!=NULL) {
                    IScenegraph *scenegraph = new Scenegraph();
                    scenegraph->makeScenegraph(root);
                    scenegraph->setMeshes(meshes);
                    scenegraph->setMeshPaths(meshPaths);

                    scenegraph->setTextureObjects(textureObjects);

                    return scenegraph;
                }
                else {
                    throw runtime_error("Parsed scene graph, but nothing set as root");
                }
            }
            protected:
                virtual void parseGroup(istream& input) {
                    string varname,name,i_light;
                    input >> varname >> name;
                    //cout << "Read " << varname << " " << name << endl;

                    vector<util::Light> lights;

                    string command;

                    input >> command;
                    addLights(command,i_light,input,lights);

                    SGNode *group = new GroupNode(name,NULL,lights);
                    nodes[varname] = group;
                }

                virtual void parseLeaf(istream& input) {
                    string varname,name,command,instanceof,i_light;
                    input >> varname >> name;
                    //cout << "Read " << varname << " " << name << endl;
                    input >> command;
                    if (command == "instanceof") {
                        input >> instanceof;
                    }

                    vector<util::Light> lights;

                    input >> command;
                    addLights(command,i_light,input,lights);

                    LeafNode *leaf = new LeafNode(instanceof,name,NULL,lights);

                    leaf->setTextureName("default");

                    nodes[varname] = leaf;
                } 

                virtual void parseScale(istream& input) {
                    string varname,name,i_light;
                    input >> varname >> name;
                    float sx,sy,sz;
                    input >> sx >> sy >> sz;

                    string command;

                    vector<util::Light> lights;

                    input >> command;
                    addLights(command,i_light,input,lights);

                    SGNode *scaleNode = new ScaleTransform(sx,sy,sz,name,NULL,lights);
                    nodes[varname] = scaleNode;
                }

                virtual void parseTranslate(istream& input) {
                    string varname,name,i_light;
                    input >> varname >> name;
                    float tx,ty,tz;
                    input >> tx >> ty >> tz;

                    string command;

                    vector<util::Light> lights;

                    input >> command;
                    addLights(command,i_light,input,lights);

                    SGNode *translateNode = new TranslateTransform(tx,ty,tz,name,NULL,lights);
                    nodes[varname] = translateNode;         
                }

                virtual void parseRotate(istream& input) {
                    string varname,name,i_light;
                    input >> varname >> name;
                    float angleInDegrees,ax,ay,az;
                    input >> angleInDegrees >> ax >> ay >> az;

                    string command;

                    vector<util::Light> lights;

                    input >> command;
                    addLights(command,i_light,input,lights);

                    SGNode *rotateNode = new RotateTransform(glm::radians(angleInDegrees),ax,ay,az,name,NULL,lights);
                    nodes[varname] = rotateNode;         
                }

                virtual void parseMaterial(istream& input) {
                    util::Material mat;
                    float r,g,b;
                    string name;
                    input >> name;
                    string command;
                    input >> command;
                    while (command!="end-material") {
                        if (command == "ambient") {
                            input >> r >> g >> b;
                            mat.setAmbient(r,g,b);
                        }
                        else if (command == "diffuse") {
                            input >> r >> g >> b;
                            mat.setDiffuse(r,g,b);
                        }
                        else if (command == "specular") {
                            input >> r >> g >> b;
                            mat.setSpecular(r,g,b);
                        }
                        else if (command == "emission") {
                            input >> r >> g >> b;
                            mat.setEmission(r,g,b);
                        }
                        else if (command == "shininess") {
                            input >> r;
                            mat.setShininess(r);
                        } else if (command == "reflection") {
                            input >> r;
                            mat.setReflection(r);
                        } else if (command == "transparency") {
                            input >> r;
                            mat.setTransparency(r);
                        } else if (command == "absorption") {
                            input >> r;
                            mat.setAbsorption(r);
                        }
                        input >> command;
                    }
                    materials[name] = mat;
                }

                virtual void parseAssignTexture(istream& input) {
                    string nodename,texturename;
                    input >> nodename >> texturename;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[nodename]);
                    if ((leafNode!=NULL) && (textureObjects.find(texturename)!=textureObjects.end())) {
                        leafNode->setTextureName(texturename);
                    }
                }

                virtual void parseTexture(istream& input) {
                    string name;
                    input >> name;
                    string filename;
                    input >> filename;
                    //cout << "path: " << filename << endl;
                    
                    ImageLoader *loader = new PPMImageLoader();
                    loader->load(filename);
                    util::TextureImage *texture = new util::TextureImage(loader->getPixels(), loader->getWidth(), loader->getHeight(), name);

                    textureObjects[name] = texture;
                }

                virtual void parseCopy(istream& input) {
                    string nodename,copyof;

                    input >> nodename >> copyof;
                    if (nodes.find(copyof)!=nodes.end()) {
                        SGNode * copy = nodes[copyof]->clone();
                        LeafNode *copyLeaf = (LeafNode*)copy;
                        copyLeaf->setTextureName("default");
                        nodes[nodename] = copyLeaf;
                    }
                }

                virtual void parseImport(istream& input) {
                    string nodename,filepath;

                    input >> nodename >> filepath;
                    ifstream external_scenegraph_file(filepath);
                    if (external_scenegraph_file.is_open()) {
                        
                        IScenegraph *importedSG = parse(external_scenegraph_file);
                        nodes[nodename] = importedSG->getRoot();
                       /* for (map<string,util::PolygonMesh<VertexAttrib> >::iterator it=importedSG->getMeshes().begin();it!=importedSG->getMeshes().end();it++) {
                            this->meshes[it->first] = it->second;
                        }
                        for (map<string,string>::iterator it=importedSG->getMeshPaths().begin();it!=importedSG->getMeshPaths().end();it++) {
                            this->meshPaths[it->first] = it->second;
                        }
                        */
                        //delete the imported scene graph but not its nodes!
                        importedSG->makeScenegraph(NULL);
                        delete importedSG;
                    }
                }

                virtual void parseAssignMaterial(istream& input) {
                    string nodename,matname;
                    input >> nodename >> matname;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[nodename]);
                    if ((leafNode!=NULL) && (materials.find(matname)!=materials.end())) {
                        leafNode->setMaterial(materials[matname]);
                    }
                }

                virtual void parseAddChild(istream& input) {
                    string childname,parentname;

                    input >> childname >> parentname;
                    ParentSGNode * parentNode = dynamic_cast<ParentSGNode *>(nodes[parentname]);
                    SGNode * childNode = NULL;
                    if (nodes.find(childname)!=nodes.end()) {
                        childNode = nodes[childname];
                    }

                    if ((parentNode!=NULL) && (childNode!=NULL)) {
                        parentNode->addChild(childNode);
                    }
                }

                virtual void parseSetRoot(istream& input) {
                    string rootname;
                    input >> rootname;

                    root = nodes[rootname];

                    //cout << "Root's name is "<< root->getName() << endl;
                }

                string stripComments(istream& input) {
                    string line;
                    stringstream clean;
                    while (getline(input,line)) {
                        int i=0;
                        while ((i<line.length()) && (line[i]!='#')) {
                            clean << line[i];
                            i++;
                        }
                        clean << endl;
                    }
                    return clean.str();
                }
            private:
                map<string,SGNode *> nodes;
                map<string,util::Material> materials;
                map<string,util::PolygonMesh<VertexAttrib> > meshes;
                map<string,string> meshPaths;
                SGNode *root;

                map<string,util::TextureImage *> textureObjects;


                // Add lights to the scene graph
                void addLights(string command, string i_light, istream& input, vector<util::Light>& lights) {
                    if (command == "add-lights") {
                        float aR,aG,aB,dR,dG,dB,sR,sG,sB,pX,pY,pZ,dX,dY,dZ,cutoff;
                        while (i_light!="end-lights") {
                            util::Light light;
                            input >> i_light;
                            if (i_light == "light") {
                                cout << "Light: " << i_light << endl;

                                input >> aR >> aG >> aB;
                                //cout << "Ambient: " << aR << aG << aB << endl;
                                light.setAmbient(aR,aG,aB);

                                input >> dR >> dG >> dB;
                                //cout << "Diffuse: " << dR << dG << dB << endl;
                                light.setDiffuse(dR,dG,dB);

                                input >> sR >> sG >> sB;
                                //cout << "Specular: " << sR << sG << sB << endl;
                                light.setSpecular(sR,sG,sB);

                                input >> pX >> pY >> pZ;
                                //cout << "Position: " << pX << pY << pZ << endl;
                                light.setPosition(pX,pY,pZ);

                                // Default values for spotlights
                                light.setSpotAngle(-1);
                                light.setSpotDirection(-100,0,-100);

                                lights.push_back(light); 
                            }
                            else if (i_light == "spotlight") {
                                //cout << "Spotlight: " << i_light << endl;

                                input >> aR >> aG >> aB;
                                //cout << "Ambient: " << aR << aG << aB << endl;
                                light.setAmbient(aR,aG,aB);

                                input >> dR >> dG >> dB;
                                //cout << "Diffuse: " << dR << dG << dB << endl;
                                light.setDiffuse(dR,dG,dB);

                                input >> sR >> sG >> sB;
                                //cout << "Specular: " << sR << sG << sB << endl;
                                light.setSpecular(sR,sG,sB);

                                input >> pX >> pY >> pZ;
                                //cout << "Position: " << pX << pY << pZ << endl;
                                light.setPosition(pX,pY,pZ);

                                input >> cutoff;
                                //cout << "Cutoff: " << cutoff << endl;
                                light.setSpotAngle(cos(glm::radians(cutoff)));

                                input >> dX >> dY >> dZ;
                                //cout << "Direction: " << dX << dY << dZ << endl;
                                light.setSpotDirection(dX,dY,dZ);

                                lights.push_back(light); 
                            }
                        }
                    }
                }
        
    };
}


#endif