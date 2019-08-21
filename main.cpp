#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "BasicTypes.hpp"
#include "BasicBMPLoader.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <chrono>
#include <thread>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

using namespace std;

GLdouble distance_ = 0.72, steeringAngle = 0.0, pivotAngle = 0.0, camHeight = 1.0, camAngle = 0.0;
float angularVelocity = 0.0f;
float sunSize = 0.15f;
array<GLfloat,3> sunPosition {-1.5f, 0.4f, 0.0f};
array<GLfloat,3> center {0.0f, 0.0f, 0.0f};

glm::vec3 camPosition(0.0, distance_, 0.0);
glm::vec3 camPivot(-0.1, distance_ - 0.08, 0.0);
glm::vec3 camPositionAux(0.0, distance_ + 0.03, 0.0);

float radius = 0.6f;

bool lifting = false, landing = false, flying = false;

vector<Object> objects;
vector<Light> lights;

#define RAD (M_PI / 180)
#define DEGREE (180 / M_PI)
#define PRINT_VALUE(value) printf("\n" #value ": %f", value)
#define PRINT_VALUEL(value) printf(" " #value ": %f", value)

void calculateCamPosition() {
    glm::mat4 camTranslateM = glm::translate(camPosition);
    glm::mat4 camTranslateMInv = glm::translate(-camPosition);
    glm::mat4 steeringM =  glm::rotate((float)-steeringAngle, camPosition);
    glm::vec3 rotationAxis = glm::cross(camPosition, camPosition - camPivot);
    glm::mat4 rotationM = glm::rotate((float)angularVelocity, rotationAxis);
    glm::mat4 pivotRotationM =  glm::rotate((float)pivotAngle, rotationAxis);
    
    camPivot = glm::vec3(rotationM * camTranslateM * pivotRotationM * steeringM * camTranslateMInv * glm::vec4(camPivot.x, camPivot.y, camPivot.z, 1));
    camPosition = glm::vec3(rotationM * glm::vec4(camPosition.x, camPosition.y, camPosition.z, 1));

    gluLookAt(camPosition.x, camPosition.y, camPosition.z, camPivot.x, camPivot.y, camPivot.z, camPosition.x, camPosition.y, camPosition.z);
}

GLdouble lerp(GLdouble a, GLdouble b, double &t) {
    return a + (b - a)*t;
}

void mouseHandler(int key, int state, int x, int y) {
    switch (key) {
    case GLUT_LEFT_BUTTON:
        lifting = (state == GLUT_DOWN);
        break;
    case GLUT_RIGHT_BUTTON:
        landing = (state == GLUT_DOWN);
    }
}

void mouseMotionHandler(int x, int y) {
    int deltaX = x - 450;
    int deltaY = y - 450;
    if(deltaY > 5) {
        pivotAngle = -0.013;
    } else if (deltaY < -5) {
        pivotAngle = 0.013;
    } else {
        pivotAngle = 0;
    }
    if(deltaX > 2) {
        steeringAngle = 0.018;
    } else if (deltaX < -2) {
        steeringAngle = -0.018;
    } else {
        steeringAngle = 0;
    }
    glutWarpPointer(450, 450);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        steeringAngle = fmod(steeringAngle - 0.005, 360);
        break;
    case GLUT_KEY_RIGHT:
        steeringAngle = fmod(steeringAngle + 0.005, 360);
        break;
    case GLUT_KEY_UP:
        angularVelocity -= 0.0035;
        break;
    case GLUT_KEY_DOWN:
        angularVelocity += 0.0035;
        break;
    }

    glutPostRedisplay();
}

void drawAxis() {
    // não há efeitos de iluminação nos eixos
    glLineWidth(3);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);

        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);

        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    glEnable(GL_LIGHTING);
}

void display(void) {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    calculateCamPosition();

    for (size_t i = 0; i < lights.size(); i++) {
        lights[i].draw();
    }
    for (size_t i = 0; i < objects.size(); i++) {
        objects[i].draw();
    }

    glutSwapBuffers();
}

void createObjects() {
    Material planetMaterial;
    planetMaterial.diffuse = {0.8, 0.8, 0.8, 1};
    planetMaterial.specular = {0.9, 0.9, 0.9, 0.3};
    planetMaterial.textureID = BitMap::loadBMP("./textures/texture.bmp").loadTexture(true);
    Planet planetModel = Planet(BitMap::loadBMP("./textures/teste - Copy.bmp"));
    objects.push_back(Object(center, planetModel, planetMaterial));

    Material skyMaterial;
    skyMaterial.textureID = BitMap::loadBMP("./textures/skybox.bmp").loadTexture(true);
    skyMaterial.emission = {0.5, 0.5, 0.5, 1};
    objects.push_back(Object(center, Planet(BitMap::loadBMP("./textures/flat.bmp"), 10.0, 1.0f), skyMaterial));
}

void createLights() {
    Material sunMaterial;
    sunMaterial.diffuse = {0.9, 0.9, 0.05, 1.0};
    sunMaterial.emission = {0.992, 0.215, 0.466, 1};
    sunMaterial.color = {1, 1, 1};
    sunMaterial.textureID = BitMap::loadBMP("./textures/sun2.bmp").loadTexture(true);
    Material lightMaterial;
    lightMaterial.specular = {0.2, 0.2, 0.2, 1};
    lightMaterial.diffuse = {0.992, 0.215, 0.466, 1};
    Light sun(sunPosition, Planet(BitMap::loadBMP("./textures/flat.bmp"), sunSize), sunMaterial, lightMaterial, GL_LIGHT1);
    sun.rotationAngle = 120.0f;
    sun.rotationAxis = {0.0f, 0.0f, 1.0f};
    lights.push_back(sun);

    Material ambientLight;
    ambientLight.ambient = {0.2, 0.2, 0.2, 1.0};
    lights.push_back(Light(sunPosition, Model(), Material(), ambientLight, GL_LIGHT0));
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_LIGHTING);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    createObjects();

    createLights();
}

void reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0, 0.02, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void idle() {
    this_thread::sleep_for(chrono::milliseconds(1000/60));

    if(lifting) {
        camPosition *= 1.004f;
        camPivot *= 1.004f;
    }
    if(landing) {
        camPosition *= 0.996f;
        camPivot *= 0.996f;
    }

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    // std::cout << "Digite a posicao do sol (Ex: 0.3 0.4 0.8):\n";
    // std::cin >> sun[0];
    // std::cin >> sun[1];
    // std::cin >> sun[2];
    // std::cout << "Digite o tamanho do planeta (Ex: 0.5):\n";
    // std::cin >> radius;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseHandler);
    glutPassiveMotionFunc(mouseMotionHandler);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}