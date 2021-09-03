#ifndef CUBESPHERE_H
#define CUBESPHERE_H

enum CubeFace {
    POSX,
    NEGX,
    POSY,
    NEGY,
    POSZ,
    NEGZ
};

float calculateCoord(glm::vec3 vec, CubeFace face, int axis);

int generateCubeSphereVertices(int subdivision) {

    int numOfFaces = 6;
    
    const float DEG2RAD = acos(-1) / 180.0f;
    glm::vec3 n1;        // normal of longitudinal plane
    glm::vec3 n2;        // normal of latitudinal plane
    glm::vec3 v;         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle
    float a2;           // latitudinal angle

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    float* vertices = new float[pointsPerRow * pointsPerRow * 3 * numOfFaces];
    unsigned int* indices = new unsigned int[(pointsPerRow-1)*(pointsPerRow-1)*2*3*numOfFaces];
    int vertexCounter = 0;
    int indexCounter = 0;
    for (int f = 0; f<numOfFaces; f++) {
        // rotate latitudinal plane from 45 to -45 degrees
        for (unsigned int i = 0; i < pointsPerRow; ++i)
        {
            // normal for latitudinal plane
            // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
            // therefore, it is rotating (0,1,0) vector by latitude angle a2
            a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
            n2[0] = -sin(a2);
            n2[1] = cos(a2);
            n2[2] = 0;


            // rotate longitudinal plane from -45 to 45
            for (unsigned int j = 0; j < pointsPerRow; ++j)
            {
                // normal for longitudinal plane
                // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
                // therefore, it is rotating (0,0,-1) vector by longitude angle a1
                a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
                n1[0] = -sin(a1);
                n1[1] = 0;
                n1[2] = -cos(a1);


                // find direction vector of intersected line, n1 x n2 and normalize direction vector
                v = glm::normalize(glm::cross(n1, n2));

                // add a vertex into array
                vertices[vertexCounter++] = calculateCoord(v, (CubeFace)f, 0);
                vertices[vertexCounter++] = calculateCoord(v, (CubeFace)f, 1);
                vertices[vertexCounter++] = calculateCoord(v, (CubeFace)f, 2);

                // generate indices for element buffer
                if (j < pointsPerRow - 1 && i < pointsPerRow - 1) {
                    indices[indexCounter++] = j + pointsPerRow * i + f*pointsPerRow*pointsPerRow;
                    indices[indexCounter++] = j + 1 + pointsPerRow * i + f * pointsPerRow * pointsPerRow;
                    indices[indexCounter++] = j + pointsPerRow * (i + 1) + f * pointsPerRow * pointsPerRow;

                    indices[indexCounter++] = j + 1 + pointsPerRow * i + f * pointsPerRow * pointsPerRow;
                    indices[indexCounter++] = j + pointsPerRow * (i + 1) + f * pointsPerRow * pointsPerRow;
                    indices[indexCounter++] = j + pointsPerRow * (i + 1) + 1 + f * pointsPerRow * pointsPerRow;
                }
            }
        }
        std::cout<< f<< " " << vertexCounter << std::endl;
        std::cout<< f << " " << indexCounter << std::endl;
    }
    
    glBufferData(GL_ARRAY_BUFFER, vertexCounter * sizeof(float), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCounter * sizeof(float), indices, GL_STATIC_DRAW);
    delete[] vertices;
    delete[] indices;
    return indexCounter;
}

float calculateCoord(glm::vec3 vec, CubeFace face, int axis) {
    switch (face) {
        case POSX:
            //x coord
            if (axis == 0) {
                return vec[0];
            }
            //y coord
            else if (axis == 1){
                return vec[1];
            }
            //z coord
            else {
                return vec[2];
            }
            break;
        case NEGX:
            //x coord
            if (axis == 0) {
                return -vec[0];
            }
            //y coord
            else if (axis == 1) {
                return vec[1];
            }
            //z coord
            else {
                return -vec[2];
            }
            break;
        case POSY:
            //x coord
            if (axis == 0) {
                return -vec[2];
            }
            //y coord
            else if (axis == 1) {
                return vec[0];
            }
            //z coord
            else {
                return -vec[1];
            }
            break;
        case NEGY:
            //x coord
            if (axis == 0) {
                return vec[2];
            }
            //y coord
            else if (axis == 1) {
                return -vec[0];
            }
            //z coord
            else {
                return -vec[1];
            }
            break;
        case POSZ:
            //x coord
            if (axis == 0) {
                return -vec[2];
            }
            //y coord
            else if (axis == 1) {
                return vec[1];
            }
            //z coord
            else {
                return vec[0];
            }
            break;
        case NEGZ:
            //x coord
            if (axis == 0) {
                return vec[2];
            }
            //y coord
            else if (axis == 1) {
                return vec[1];
            }
            //z coord
            else {
                return -vec[0];
            }
            break;
    }
    
}

#endif
