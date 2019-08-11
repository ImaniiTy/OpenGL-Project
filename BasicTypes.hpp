#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

#include <GL/glut.h>
#include <array>
#include <vector>
#include "BasicBMPLoader.hpp"

using namespace std;

struct Material {
    array<GLfloat,4> specular {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,4> diffuse {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,4> ambient {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,4> emission {0.0f, 0.0f, 0.0f, 1.0f};
    array<GLfloat,1> shininess {50.0f};
    array<GLfloat,3> color {0.0f, 0.0f, 0.0f};
    GLuint textureID;
};

class Model {
    public:
        GLdouble radius = -1.0;
        GLint vResolution;
        GLint hResolution;
        vector<float> vertices;
        vector<float> normals;
        vector<float> texCoords;
        vector<unsigned int> indices;
        BitMap heightMap;

        Model(GLdouble radius, GLint vResolution, GLint hResolution);
        Model();

        void render();

        int size() {
            return indices.size();
        };
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

class Planet : public Model {
    private:
        std::vector<unsigned int> lineIndices;
        

    public:
        Planet(BitMap heightMap);

        void render();

    public:
        void buildVerticesSphere();    
};

#endif