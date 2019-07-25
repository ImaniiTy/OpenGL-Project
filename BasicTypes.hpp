#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

#include <GL/glut.h>
#include <array>

using namespace std;

struct Material {
    array<GLfloat,4> specular {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,4> diffuse {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,4> ambient {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,4> emission {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,1> shininess {50.0f};
    array<GLfloat,3> color {0.0f, 0.0f, 0.0f};
};

class Model {
    public:
        GLdouble radius = -1.0;
        GLint vResolution;
        GLint hResolution;

        Model(GLdouble radius, GLint vResolution, GLint hResolution);
        Model();

        void render();
};

class Object {
    protected:
        Model model;
        Material material;
        array<GLfloat,3> position;

    private:
        void applyMaterials();

    public:
        Object(array<GLfloat,3> position, Model model, Material material);

        Material getMaterial();
        Model getModel();

        array<GLfloat,3> getPosition();
        void setPosition(array<GLfloat,3> newPosition);

        void draw();

};

class Light : public Object {
    protected:
        Material lightMaterial;

    private:
        void applyLightMaterials();

    public:
        int id;
        Light(array<GLfloat,3> position, Model model, Material material, Material lightMaterial, int id);

        void draw();
};

#endif