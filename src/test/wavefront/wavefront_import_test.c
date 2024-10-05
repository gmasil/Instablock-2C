#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavefront.h"

int main(void) {
    int loaded_faces;
    char *filename = "demo.obj";
    float *buffer  = loadWavefrontObj(filename, &loaded_faces);
    free(buffer);
    if (loaded_faces != 400) {
        printf("Expected to load %d faces from %s!\n", loaded_faces, filename);
        return 1;
    }
    return 0;
}
