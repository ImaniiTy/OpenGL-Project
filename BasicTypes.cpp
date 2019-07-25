#include <GL/glut.h>
#include <GL/gl.h>
#include "BasicTypes.hpp"
#include <iostream>

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
    glPushAttrib(GL_LIGHTING_BIT);
        applyMaterials();
        glPushMatrix();
            glTranslatef(position[0], position[1], position[2]);
            model.render();
        glPopMatrix();
    glPopAttrib();
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

Model::Model(){}

void Model::render() {
    if(radius > 0){
        glutSolidSphere(radius, vResolution, hResolution);
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

// gluLookAt (posCameraX, posCameraY, posCameraZ, offset[0], -0.8 * sin((phi) * RAD), 0.8 * cos((phi) * RAD), posCameraX, posCameraY, posCameraZ);

