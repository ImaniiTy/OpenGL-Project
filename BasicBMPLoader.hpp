#ifndef BASIC_BMP_LOADER_H_
#define BASIC_BMP_LOADER_H_

#include <stdint.h>
#include <iostream>
#include <fstream>

using namespace std;

class BitMap {
    public:
        uint32_t width;
        uint32_t height;
        uint8_t *data;

        static BitMap loadBMP(const char* path) {
            BitMap bm;
            uint8_t info[54];
            uint8_t *data;

            ifstream file(path, ios::binary);
            file.read((char*)info, 54);
            

            uint32_t sizeImage = *(uint32_t*)&info[34];
            uint32_t dataOffset = *(uint32_t*)&info[10];
            uint32_t width = *(uint32_t*)&info[18];
            uint32_t height = *(uint32_t*)&info[22];

            file.seekg(dataOffset);
            data = new uint8_t[sizeImage];
            file.read((char*)data, sizeImage);

            bm.width = width;
            bm.height = height;
            bm.data = data;

            // cout << (int) bm.data[(79*112 + 1) * 3] << endl;

            return bm;
        }

        uint8_t getPixel(uint32_t x, uint32_t y) {
            return data[(x*width + y) * 3];
        }

        GLuint loadTexture(bool wrap) {
            GLenum type = GL_UNSIGNED_BYTE;    // only allow BMP with 8-bit per channel

            // We assume the image is 8-bit, 24-bit or 32-bit BMP
            GLenum format = GL_RGB;

            // gen texture ID
            GLuint texture;
            glGenTextures(1, &texture);

            // set active texture and configure it
            glBindTexture(GL_TEXTURE_2D, texture);

            // select modulate to mix texture with color for shading
            //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

            // if wrap is true, the texture wraps over at the edges (repeat)
            //       ... false, the texture ends at the edges (clamp)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
            //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // copy texture data
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
            //glGenerateMipmap(GL_TEXTURE_2D);

            // build our texture mipmaps
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, type, data);

            return texture;
        }
};

#endif