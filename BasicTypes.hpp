#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

#include <GL/glut.h>
#include <array>
#include <vector>
#include "BasicBMPLoader.hpp"
#include "glm/glm.hpp"

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
        GLdouble radius = 0.6;
        GLint vResolution = -1;
        GLint hResolution = -1;
        vector<float> vertices;
        vector<float> normals;
        vector<float> texCoords;
        vector<unsigned int> indices;
        BitMap heightMap;
        GLUquadric *quadratic = NULL;
        float normalDirection = 1.0f;

        Model(GLdouble radius, GLint vResolution, GLint hResolution);
        Model();

        void render();

        int size() {
            return indices.size();
        };

        void rebuildTextureCoords(); 
};

class Object {
    protected:
        Model model;
        Material material;
        array<GLfloat,3> position;

    public:
        glm::vec3 getPosition();
        array<GLfloat,3> rotationAxis;
        GLfloat rotationAngle = 0.0f;

    private:
        void applyMaterials();

    public:
        Object(array<GLfloat,3> position, Model model, Material material);

        Material getMaterial();
        Model& getModel();

        void setModel(Model model_);


        void setPosition(glm::vec3 newPosition);

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
        Planet(BitMap heightMap_, GLdouble radius);
        Planet(BitMap heightMap_, GLdouble radius, float normalDirection);

        void render();

    public:
        void buildVerticesSphere();
};

#endif