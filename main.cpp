#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "BasicTypes.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <array>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

GLdouble posCameraX,posCameraY,posCameraZ;
GLdouble initialDistance = 0.53,distance_ = 0.53, phi = 1, theta = 0, offsetTheta = 0, camAngleOffset = 40.0;
GLdouble angularVelocity = 0, step = 0.0;
array<GLfloat,3> sun {0.3, 0.4, 0.8};
array<GLfloat,3> position {0.0, 0.0, 0.0};

float radius = 0.5;
double xDirection = 1, yDirection = 1;

bool lifting = false, landing = false, flying = false;

vector<Object> objects;
vector<Light> lights;

#define RAD (M_PI / 180)
#define PRINT_VALUE(value) printf("\n" #value ": %f", value)

void updateCamPosition(void) {
    posCameraY = distance_ * cos(phi * RAD);
    posCameraZ = distance_ * sin(phi * RAD) * cos(theta * RAD);
    posCameraX = distance_ * sin(phi * RAD) * sin(theta * RAD);
}

array<GLdouble, 3> calculatePosition(GLdouble phi, GLdouble theta) {
    array<GLdouble, 3> position;

    position[1] = distance_ * cos(phi * RAD);
    position[2] = distance_ * sin(phi * RAD) * cos(theta * RAD);
    position[0] = distance_ * sin(phi * RAD) * sin(theta * RAD);

    return position;
}

void calculateCamPosition() {
    array<GLdouble, 3> offset = calculatePosition(phi + camAngleOffset, theta);
    gluLookAt(posCameraX, posCameraY, posCameraZ, offset[0], offset[1], offset[2], posCameraX, posCameraY, posCameraZ);
    glRotatef(offsetTheta, posCameraX, posCameraY, posCameraZ);
    
}

GLdouble lerp(GLdouble a, GLdouble b, double &t) {
    return a + (b - a)*t;
}

void mouseHandler(int key, int state, int x, int y) {
    switch (key) {
    case GLUT_LEFT_BUTTON:
        if(!landing && !flying){
            lifting = true;
            angularVelocity += 0.1;
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if(!lifting && flying){
            landing = true;
        }
        break;
    }

    updateCamPosition();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        offsetTheta = fmod(offsetTheta - 2.0, 360);
        break;
    case GLUT_KEY_RIGHT:
        offsetTheta = fmod(offsetTheta + 2.0, 360);
        break;
    case GLUT_KEY_UP:
        // phi = fmod(phi + 2, 360);
        angularVelocity += 0.1;
        break;
    case GLUT_KEY_DOWN:
        angularVelocity -= 0.1;
        break;
    case GLUT_LEFT_BUTTON:
        distance_ += 0.1;
        break;
    }

    updateCamPosition();
    PRINT_VALUE(distance_);
    std::cout << "\nX: "<< posCameraX << "\nY: " << posCameraY << "\nZ: "<< posCameraZ;
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
    objects.push_back(Object(position, Model(radius, 200, 200), planetMaterial));
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
    ambientLight.ambient = {0.3, 0.3, 0.3, 1};
    ambientLight.specular = {0.1, 0.1, 0.1, 1};
    lights.push_back(Light(sun, Model(), Material(), ambientLight, GL_LIGHT0));
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_LIGHTING);

    //light();
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    updateCamPosition();

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
        distance_ = lerp(initialDistance, 0.6, step);
        step+= 0.005;
        if(step >= 1.0) {
            step = 0;
            lifting = false;
            flying = true;
        }
    }
    if(landing) {
        distance_ = lerp(0.6, initialDistance, step);
        step+= 0.005;
        if(step >= 1.0) {
            step = 0;
            landing = false;
            flying = false;
        }
    }
    updateCamPosition();
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
