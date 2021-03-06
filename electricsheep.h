//
//  electricsheep.h
//  electricSheep
//
//  Created by Ali Helmy on 23/10/2012.
//  Copyright (c) 2012 wackyCube. All rights reserved.
//

#ifndef electricSheep_electricsheep_h
#define electricSheep_electricsheep_h

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GLIncludes.h"
#include "shaderUtilities.h"
#include "GameObject.h"
#include <vector>

class ElectricSheepEngine {
    
public:
    ElectricSheepEngine(float width, float height);
    ~ElectricSheepEngine();
    void initWorld();
    void update(GLfloat elapsedTime);
    void render();
    void reshape(int newWindowWidth, int newWindowHeight);
    void panCamera(GLfloat horizontal, GLfloat vertical);
    void zoomCamera(GLfloat scale);

private:
    GLuint shaderProgram;
    GLint shaderAttribute_coord3D;
    GLint shaderAttribute_vertexColour;
    GLint shaderAttribute_normal;
    GLint shaderAttribute_uniform_model;
    GLint shaderAttribute_uniform_inverseTransposeModel;
    GLint shaderAttribute_uniform_view;
    GLint shaderAttribute_uniform_projection;
    
    //view matrix using look at
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
    glm::mat4 view;
    
    //projection matrix
    GLfloat lensAngle;
    GLfloat aspectRatio;
    GLfloat nearClippingPlane;
    GLfloat farClippingPlane;
    glm::mat4 projection;
    
    std::vector<GameObject *> herd;
    
    bool initShaders(const char *vertexShaderSource, const char *fragmentShaderSource);
    void initCamera();
    void updateCamera();
    
    void initProjection();
    
    void freeResources();
    
    void renderObjectModel(Model *model, glm::mat4 modelMatrix);
};

#endif
