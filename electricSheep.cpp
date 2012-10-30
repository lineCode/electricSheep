#include "electricSheep.h"

//windowSize
int windowWidth=1136;
int windowHeight=640;

ElectricSheepEngine::ElectricSheepEngine() {
    models=std::vector<ObjModel>();
}

ElectricSheepEngine::~ElectricSheepEngine() {
    freeResources();
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
    
    const char *vertexColourAttributeName="vertexColour";
    if(!bindShaderAttribute(&shaderAttribute_vertexColour, shaderProgram, vertexColourAttributeName)){
        fprintf(stderr, "Could not bind shader attribute %s\n", vertexColourAttributeName);
        return false;
    }
    
    const char *mvpMatrixAttributeName="mvp";
    if(!bindShaderUniformAttribute(&shaderAttribute_uniform_mvp, shaderProgram, mvpMatrixAttributeName)){
        fprintf(stderr, "Could not bind shader attribute %s\n", mvpMatrixAttributeName);
        return false;
    }
    
    const char *animationMatrixAttributeName="animation";
    if(!bindShaderUniformAttribute(&shaderAttribute_uniform_animation, shaderProgram, animationMatrixAttributeName)){
        fprintf(stderr, "Could not bind shader attribute %s\n", animationMatrixAttributeName);
        return false;
    }
    
    //if nothing fails till here, shader init is a success
    return true;
}

void ElectricSheepEngine::initModels() {
    ObjModel *model=new ObjModel("");
    models.push_back(*model);
}

void ElectricSheepEngine::freeResources() {
    glDeleteProgram(shaderProgram);
#pragma message("TODO: delete model data")
    models.clear();
}

void ElectricSheepEngine::render() {
    //clear screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //use the shader
    glUseProgram(shaderProgram);
    
    //enable attributes in program
    glEnableVertexAttribArray(shaderAttribute_coord3D);
    glEnableVertexAttribArray(shaderAttribute_vertexColour);
    
    for (std::vector<ObjModel>::iterator i=models.begin(); i!=models.end(); ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, i->getVerticesBufferObject());
        glVertexAttribPointer(shaderAttribute_coord3D, // attribute
                              3, // number of elements per vertex, here (x,y)
                              GL_FLOAT, // the type of each element
                              GL_FALSE, // take our values as-is
                              sizeof(struct modelData), // 2d coord every 5 elements
                              0 // offset of first element
                              );
        
        glBindBuffer(GL_ARRAY_BUFFER, i->getVerticesBufferObject());
        glVertexAttribPointer(shaderAttribute_vertexColour, // attribute
                              3, // number of elements per vertex, here (r,g,b)
                              GL_FLOAT, // the type of each element
                              GL_FALSE, // take our values as-is
                              sizeof(struct modelData), // colour every 5 elements
                              (GLvoid *)(offsetof(struct modelData, colour3D)) // skip 2d coords
                              );
        
        //draw the cube by going through its elements array
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i->getFacesBufferObject());
        int bufferSize;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        glDrawElements(GL_TRIANGLES, bufferSize/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    }
    
    //close up the attribute in program, no more need
    glDisableVertexAttribArray(shaderAttribute_coord3D);
    glDisableVertexAttribArray(shaderAttribute_vertexColour);
}

void ElectricSheepEngine::reshape(int newWindowWidth, int newWindowHeight)
{
    windowWidth=newWindowWidth;
    windowHeight=newWindowHeight;
    glViewport(0, 0, windowWidth, windowHeight);
}

void ElectricSheepEngine::update()
{
#pragma message("TODO: update game state")
}