#ifndef CUBESPHERE_H
#define CUBESPHERE_H

int generateVertices(int subdivision, unsigned int* VAO, unsigned int* VBO, unsigned int* EBO) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    const float DEG2RAD = acos(-1) / 180.0f;
    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along Y-axis
    float a2;           // latitudinal angle along Z-axis

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    float* vertices = new float[pointsPerRow * pointsPerRow * 3];
    unsigned int* indices = new unsigned int[(pointsPerRow-1)*(pointsPerRow-1)*2*3];
    int vertexCounter = 0;
    int indexCounter = 0;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for (unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for (unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into array
            vertices[vertexCounter++] = v[0];
            vertices[vertexCounter++] = v[1];
            vertices[vertexCounter++] = v[2];

            if (j < pointsPerRow - 1 && i < pointsPerRow - 1) {
                indices[indexCounter++] = j + pointsPerRow*i;
                indices[indexCounter++] = j + 1 + pointsPerRow * i;
                indices[indexCounter++] = j + pointsPerRow * (i + 1);

                indices[indexCounter++] = j + 1 + pointsPerRow * i;
                indices[indexCounter++] = j + pointsPerRow * (i + 1);
                indices[indexCounter++] = j + pointsPerRow * (i + 1) + 1;
            }
        }
    }
   
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCounter * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCounter * sizeof(float), indices, GL_STATIC_DRAW);
    delete[] vertices;
    delete[] indices;
    return indexCounter;
};

#endif
