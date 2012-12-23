#include "electricSheep.h"
#include "Path_C_Interface.h"

//windowSize
int windowWidth=1136;
int windowHeight=640;

ElectricSheepEngine::ElectricSheepEngine(float width, float height) {
    reshape(width, height);
    
    initShaders(pathForFile("vertex", "glsl"), pathForFile("fragment", "glsl"));
    initCamera();
    initProjection();
}

ElectricSheepEngine::~ElectricSheepEngine() {
    freeResources();
}

void ElectricSheepEngine::initCamera() {
    cameraPosition=glm::vec3(4,4,2);
    cameraTarget=glm::vec3(0,0,0);
    cameraUp=glm::vec3(0,0,1);
    glm::vec3 cameraDirection=glm::normalize(cameraPosition-cameraTarget);
    cameraRight=glm::cross(cameraDirection, cameraUp);
    cameraRight.z=0;
    updateCamera();
}

void ElectricSheepEngine::updateCamera() {
    view=glm::lookAt(cameraPosition, cameraTarget, cameraUp);
}

void ElectricSheepEngine::initProjection() {
    lensAngle=45.0f;
    aspectRatio=1.0*(windowWidth/windowHeight);
    nearClippingPlane=0.1f;
    farClippingPlane=100.0f;
    projection=glm::perspective(lensAngle, aspectRatio, nearClippingPlane, farClippingPlane);
}

bool ElectricSheepEngine::initShaders(const char *vertexShaderPath, const char *fragmentShaderPath) {
    //load shaders & compile
    GLint linkSuccess=GL_FALSE;
    
    //vertex shader
    GLuint vertexShaderId=createShader(vertexShaderPath, GL_VERTEX_SHADER);
    
    //fragment shader
    GLuint fragmentShaderId=createShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
    
    //program
    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkSuccess);
    if(linkSuccess==GL_FALSE){
        fprintf(stderr, "Shader Linking Failure\n");
        return false;
    }
    
    //shader attributes
    const char *coord2DAttributeName="coord3D";
    if(!bindShaderAttribute(&shaderAttribute_coord3D, shaderProgram, coord2DAttributeName)){
        fprintf(stderr, "Could not bind shader attribute %s\n", coord2DAttributeName);
        return false;
    }
    
    const char *texCoordInAttributeName="TexCoordIn";
    if(!bindShaderAttribute(&shaderAttribute_TexCoordIn, shaderProgram, texCoordInAttributeName)){
        fprintf(stderr, "Could not bind shader attribute %s\n", texCoordInAttributeName);
        return false;
    }
    
    const char *mvpMatrixAttributeName="mvp";
    if(!bindShaderUniformAttribute(&shaderAttribute_uniform_mvp, shaderProgram, mvpMatrixAttributeName)){
        fprintf(stderr, "Could not bind shader attribute %s\n", mvpMatrixAttributeName);
        return false;
    }
    
    const char *textureAttributeName="Texture";
    if(!bindShaderUniformAttribute(&shaderAttribute_uniform_Texture, shaderProgram, textureAttributeName)){
        fprintf(stderr, "Could not bind shader attribute %s\n", textureAttributeName);
        return false;
    }
    
    //if nothing fails till here, shader init is a success
    return true;
}

#define numberOfSheep   1

void ElectricSheepEngine::initWorld() {
    for (int s=0; s<numberOfSheep; s++) {
        herd.push_back(new Voxel());
    }
}

void ElectricSheepEngine::freeResources() {
    glDeleteProgram(shaderProgram);
    herd.clear();
}

void ElectricSheepEngine::render() {
    //clear screen
    glClearColor(0.52, 0.8, 0.97, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    //use the shader
    glUseProgram(shaderProgram);
    
    //enable attributes in program
    glEnableVertexAttribArray(shaderAttribute_coord3D);
    glEnableVertexAttribArray(shaderAttribute_TexCoordIn);
    
    for (int i=0; i!=numberOfSheep; i++) {
        Voxel *voxel=herd[i];
        Model *model=voxel->getModel();
        
        renderObjectModel(model, voxel->getModelMatrix());
    }
    
    //close up the attribute in program, no more need
    glDisableVertexAttribArray(shaderAttribute_coord3D);
    glDisableVertexAttribArray(shaderAttribute_TexCoordIn);
}

void ElectricSheepEngine::renderObjectModel(Model *model, glm::mat4 modelMatrix) {
    //model matrix using model position vector
    glm::mat4 mvp=projection*view*modelMatrix;
    glUniformMatrix4fv(shaderAttribute_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    
    glBindBuffer(GL_ARRAY_BUFFER, model->getVerticesBufferObject());
    glVertexAttribPointer(shaderAttribute_coord3D, // attribute
                          3, // number of elements per vertex, here (x,y)
                          GL_FLOAT, // the type of each element
                          GL_FALSE, // take our values as-is
                          sizeof(struct modelData), // coord every (sizeof) elements
                          0 // offset of first element
                          );
    
    glVertexAttribPointer(shaderAttribute_TexCoordIn, // attribute
                          2, // number of elements per vertex, here (r,g,b)
                          GL_FLOAT, // the type of each element
                          GL_FALSE, // take our values as-is
                          sizeof(struct modelData), // // coord every (sizeof) elements
                          (GLvoid *)(offsetof(struct modelData, texCoords)) // skip coords & colour coords
                          );
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->getTextureID());
    glUniform1i(shaderAttribute_uniform_Texture, 0);
    
    //draw the model by going through its elements array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getFacesBufferObject());
    int bufferSize;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    glDrawElements(GL_TRIANGLES, bufferSize/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}

void ElectricSheepEngine::zoomCamera(GLfloat scale) {
    cameraPosition.x*=scale;
    cameraPosition.y*=scale;
    cameraPosition.z*=scale;
    updateCamera();
}

void ElectricSheepEngine::panCamera(GLfloat horizontal, GLfloat vertical) {
    GLfloat yawAngle=horizontal*-0.5;
    GLfloat pitchAngle=vertical*0.5;
    
    glm::mat4 yawRotation=glm::rotate(glm::mat4(1.0f), yawAngle, cameraUp);
    glm::mat4 pitchRotation=glm::rotate(glm::mat4(1.0f), pitchAngle, cameraRight);
    
    glm::vec3 camToTarget=cameraPosition-cameraTarget;
    glm::vec4 camToTarget4(camToTarget.x, camToTarget.y, camToTarget.z, 1.0);
    
    camToTarget4=yawRotation*pitchRotation*camToTarget4;
    
    camToTarget.x=camToTarget4.x;
    camToTarget.y=camToTarget4.y;
    camToTarget.z=camToTarget4.z;
    
    cameraPosition.x=camToTarget.x+cameraTarget.x;
    cameraPosition.y=camToTarget.y+cameraTarget.y;
    cameraPosition.z=camToTarget.z+cameraTarget.z;
    
    glm::vec3 cameraDirection=glm::normalize(cameraPosition-cameraTarget);
    cameraRight=glm::cross(cameraDirection, cameraUp);
    cameraRight.z=0;
    cameraUp=glm::cross(cameraRight, cameraDirection);
    
    updateCamera();
}

void ElectricSheepEngine::reshape(int newWindowWidth, int newWindowHeight)
{
    windowWidth=newWindowWidth;
    windowHeight=newWindowHeight;
    glViewport(0, 0, windowWidth, windowHeight);
}

void ElectricSheepEngine::update(GLfloat elapsedTime) {
    for (int i=0; i!=numberOfSheep; i++) {
        Voxel *voxel=herd[i];
        voxel->update(elapsedTime);
    }
}