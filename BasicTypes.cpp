#include <GL/glut.h>
#include <GL/gl.h>
#include "BasicTypes.hpp"
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

#define PRINT_VALUE(value) printf("\n" #value ": %f", value)

//-----Object-----
Object::Object(array<GLfloat,3> position, Model model, Material material)
    : position(position), model(model), material(material) {
    
    //applyMaterials();
}

void Object::applyMaterials() {
    glMaterialfv(GL_FRONT, GL_SPECULAR, material.specular.data());
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material.diffuse.data());
    glMaterialfv(GL_FRONT, GL_EMISSION, material.emission.data());
    glMaterialfv(GL_FRONT, GL_SHININESS, material.shininess.data());
    glColor3fv(material.color.data());
}

void Object::draw() {
    if(!model.vertices.empty()){
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, model.vertices.data());
    }
    if(!model.normals.empty()){
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, model.normals.data());
    }
    if(!model.texCoords.empty()){
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindTexture(GL_TEXTURE_2D, material.textureID);
        glTexCoordPointer(2, GL_FLOAT, 0, model.texCoords.data());
    }
    glPushAttrib(GL_LIGHTING_BIT);
        applyMaterials();
        glPushMatrix();
            glTranslatef(position[0], position[1], position[2]);
            model.render();
        glPopMatrix();
    glPopAttrib();

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

Material Object::getMaterial(){
    return material;
}

Model Object::getModel() {
    return model;
}

array<GLfloat,3> Object::getPosition() {
    return position;
}

void Object::setPosition(array<GLfloat,3> newPosition) {
    position = newPosition;
}
//-----Object-----

//-----Model-----
Model::Model(GLdouble radius, GLint vResolution, GLint hResolution) 
    : radius(radius), vResolution(vResolution), hResolution(hResolution) {}

Model::Model() {}

void Model::render() {
    if(radius > 0){
        glutSolidSphere(radius, vResolution, hResolution);
    } else {
        glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT,  indices.data());
    }
}
//-----Model-----

//-----Light-----
Light::Light(array<GLfloat,3> position, Model model, Material material, Material lightMaterial, int id) 
    : Object(position, model, material), lightMaterial(lightMaterial), id(id) {}

void Light::applyLightMaterials() {
    glLightfv(id, GL_SPECULAR, lightMaterial.specular.data());
    glLightfv(id, GL_DIFFUSE, lightMaterial.diffuse.data());
    glLightfv(id, GL_AMBIENT, lightMaterial.ambient.data());

    glEnable(id);
}

void Light::draw() {
    // glPushAttrib(GL_LIGHTING_BIT);
    applyLightMaterials();
    glPushMatrix();
        glLightfv(GL_LIGHT1, GL_POSITION, position.data());
    glPopMatrix();
    // glPopAttrib();
    Object::draw();
}
//-----Light-----

//-----Planet-----
Planet::Planet(BitMap heightMap_)
    :Model() {
    heightMap = heightMap_;
    buildVerticesSphere();
    cout << size() << endl;
}

void Planet::render() {
    glColor3f (1, 1, 1);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT,  indices.data());
}

void Planet::buildVerticesSphere() {
    const float PI = 3.1415926f;
    int sectorCount = heightMap.width;
    int stackCount = heightMap.height;
    float radius = 0.4;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
    float s, t;                                     // texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)
        // i = (PI / 2 - stackAngle) / (PI / heightMap.height)
        // j = sectorangle / (2 * PI / heightMap.width)
        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        float firstHeight;
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

            // normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            //valor que eu inseri para perturbar a superfície da esfera. 
            //Vocês devem usar valores de um mapa de altitude (heightmap) no lugar de height.
            float height = ((float) heightMap.getPixel(i % stackCount, j % sectorCount) / 255) * 0.5;
            if (j == 0){
                firstHeight = height;
            } 
            if (j == sectorCount) {
                height = firstHeight;
            }
            //desloca no eixo da normal e insere as novas coordenadas no array de posições dos vértices
            vertices.push_back(x+nx*height);
            vertices.push_back(y+ny*height);
            vertices.push_back(z+nz*height);
            
            //adiciona vetores normais ao array de normais
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // vertex tex coord between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {   
        k1 = i * (sectorCount + 1);    // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack
        for(int j = 0; j <= sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if(i != 0)
            {
            // k1---k2---k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1+1);               
            }

            if(i != (stackCount-1))
            {
            // k1+1---k2---k2+1
                indices.push_back(k1+1);
                indices.push_back(k2);
                indices.push_back(k2+1);
            }

            // vertical lines for all stacks
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            if(i != 0)  // horizontal lines except 1st stack
            {
                lineIndices.push_back(k1);
                lineIndices.push_back(k1 + 1);
            }
        }
    }

    cout << indices.size() << endl;
}
//-----Planet-----

// gluLookAt (posCameraX, posCameraY, posCameraZ, offset[0], -0.8 * sin((phi) * RAD), 0.8 * cos((phi) * RAD), posCameraX, posCameraY, posCameraZ);
