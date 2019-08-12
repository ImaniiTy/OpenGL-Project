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

GLdouble distance_ = 0.72, phi = 180, theta = 0, steeringAngle = 0, camAngle = 20.0;
GLdouble angularVelocity = 0.0, step = 0.0;
array<GLfloat,3> sun {0.3, 0.4, 0.8};
array<GLfloat,3> position {0.0, 0.0, 0.0};

glm::vec3 camPosition(0.0, distance_, 0.0);
glm::vec3 camPivot(0.0, distance_ - 0.08, 0.1);
glm::vec3 camPositionAux(0.0, distance_ + 0.03, 0.0);

float radius = 0.6;
double xDirection = 1, yDirection = 1;

bool lifting = false, landing = false, flying = false;

Planet flat = Planet(BitMap::loadBMP("./textures/flat.bmp"));
vector<Object> objects;
vector<Light> lights;

#define RAD (M_PI / 180)
#define PRINT_VALUE(value) printf("\n" #value ": %f", value)
#define PRINT_VALUEL(value) printf(" " #value ": %f", value)

void calculateCamPosition() {
    BitMap heightMap = objects[0].getModel().heightMap;
    int i = (phi / 360) * heightMap.width;
    int j = (steeringAngle / 360) * heightMap.height;
    // cout << "\r" << "phi: "<< phi << "  i: " << i
    //      << " | steeringAngle: " << steeringAngle << "  j: " << j 
    //      << " | Pixel: " << (int) heightMap.getPixel(i % heightMap.height, j % heightMap.width) << flush;
    // cout << endl;
    float height = ((float) heightMap.getPixel(i % heightMap.height, j % heightMap.width) / 255) * 0.5;
    glm::mat4 camTranslateM = glm::translate(camPosition);
    glm::mat4 pivotTranslateM = glm::translate(camPosition);
    glm::mat4 steeringM =  glm::rotate((float)steeringAngle, camPosition);
    glm::vec3 rotationAxis = glm::cross(camPosition, camPivot - camPosition);
    glm::mat4 rotationM = glm::rotate((float)angularVelocity, rotationAxis);

    camPosition = glm::vec3(glm::vec4(camPosition.x, camPosition.y, camPosition.z, 1) * rotationM * camTranslateM);
    camPivot = glm::vec3(glm::vec4(camPivot.x, camPivot.y, camPivot.z, 1) * rotationM * steeringM * pivotTranslateM);

    gluLookAt(camPosition.x, camPosition.y, camPosition.z, camPivot.x, camPivot.y, camPivot.z, camPosition.x, camPosition.y, camPosition.z);
}

GLdouble lerp(GLdouble a, GLdouble b, double &t) {
    return a + (b - a)*t;
}

void mouseHandler(int key, int state, int x, int y) {
    switch (key) {
    case GLUT_LEFT_BUTTON:
        if(!landing){
            lifting = true;
            // angularVelocity -= 0.01;
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if(!lifting){
            landing = true;
        }
        break;
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        // steeringAngle = fmod(steeringAngle - 2.0, 360) > 0 ? fmod(steeringAngle - 2.0, 360) : 360 - fmod(steeringAngle - 2.0, 360);
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
    
    drawAxis();
    lights[0].draw();
    lights[1].draw();
    objects[0].draw();

    glutSwapBuffers();
}

void createObjects() {
    Material planetMaterial;
    planetMaterial.diffuse = {0.3, 0.3, 0.3, 1};
    planetMaterial.specular = {0.9, 0.9, 0.9, 0.3};
    planetMaterial.textureID = BitMap::loadBMP("./textures/texture.bmp").loadTexture(true);
    Planet planetModel = Planet(BitMap::loadBMP("./textures/teste.bmp"));
    objects.push_back(Object(position, planetModel, planetMaterial));
}

void createLights() {
    Material sunMaterial;
    sunMaterial.diffuse = {0.7, 0.7, 0.7, 1.0};
    sunMaterial.emission = {1, 1, 0, 1};
    sunMaterial.color = {0, 1, 1};
    Material lightMaterial;
    lightMaterial.specular = {0.2, 0.2, 0.2, 1};
    lightMaterial.diffuse = {0.7, 0.7, 0.5, 1};
    lights.push_back(Light(sun, Model(0.08, 50, 50), sunMaterial, lightMaterial, GL_LIGHT1));

    Material ambientLight;
    ambientLight.ambient = {0.9, 0.9, 0.9, 1};
    ambientLight.specular = {0.1, 0.1, 0.1, 1};
    lights.push_back(Light(sun, Model(), Material(), ambientLight, GL_LIGHT0));
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
    phi = fmod(phi + angularVelocity, 360);
    if(lifting) {
        if(glm::length(camPosition) >= glm::length(camPositionAux)) {
            lifting = false;
            camPositionAux = glm::vec3(0.0f, distance_, 0.0f);
        } else {
            camPosition = camPosition * 1.001f; 
        }
    }
    if(landing) {
        if(glm::length(camPosition) <= glm::length(camPositionAux)) {
            landing = false;
            camPositionAux = glm::vec3(0.0f, distance_ + 0.03, 0.0f);
        } else {
            camPosition = camPosition * 0.999f; 
            cout << "\r" << camPosition.y << " | " << camPositionAux.y << flush;
        }
    }
    calculateCamPosition();
    // Model planet = objects[0].getModel();
    // planet.vertices[(64*64*camPosition.x + 64*camPosition.y + camPosition.z) * 3] = flat.vertices[(64*64*camPosition.x + 64*camPosition.y + camPosition.z) * 3];
    // planet.vertices[(64*64*camPosition.x + 64*camPosition.y + camPosition.z) * 3 + 1] = flat.vertices[(64*64*camPosition.x + 64*camPosition.y + camPosition.z) * 3 + 1];
    // planet.vertices[(64*64*camPosition.x + 64*camPosition.y + camPosition.z) * 3 + 2] = flat.vertices[(64*64*camPosition.x + 64*camPosition.y + camPosition.z) * 3 + 2];
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
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
