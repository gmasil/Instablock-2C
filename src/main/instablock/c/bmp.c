#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

GLuint load_bmp(char *filename, int filter) {
    printf("Reading bmp image from %s\n", filename);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int bpp;
    unsigned int bpp_prefix;
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;
    // Actual RGB data
    unsigned char *data;

    // Open the file
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("File %s could not be opened.\n", filename);
        return 0;
    }

    // Read the header, i.e. the 54 first bytes

    // If less than 54 bytes are read, problem
    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    // A BMP files always begins with "BM"
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }

    // Read the information about the image
    bpp        = *(int *)&(header[0x1C]);
    bpp_prefix = *(int *)&(header[0x1E]);
    dataPos    = *(int *)&(header[0x0A]);
    imageSize  = *(int *)&(header[0x22]);
    width      = *(int *)&(header[0x12]);
    height     = *(int *)&(header[0x16]);

    // Make sure this is a 24bpp file
    if (bpp_prefix != 0) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    if (bpp != 24) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0) {
        imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
    }
    if (dataPos == 0) {
        dataPos = 54; // The BMP header is done that way
    }
    // bytes between header and actual data
    int seek_amount = dataPos - 54;

    // Create a buffer
    data = malloc(imageSize);

    // seek forward if there is a gap between header and data
    if (seek_amount > 0) {
        fseek(file, seek_amount, SEEK_CUR);
    }
    // Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);

    // Everything is in memory now, the file can be closed.
    fclose(file);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify
    // this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    // OpenGL has now copied the data. Free our own version
    free(data);

    // trilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (filter == 0) {
        // minecraft style
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } else {
        // trilinear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // mipmaps required by trilinear filtering. Generate them automatically.
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // Return the ID of the texture we just created
    return textureID;
}
