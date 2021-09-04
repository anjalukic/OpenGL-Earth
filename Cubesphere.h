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

class Cubesphere {
public:
    unsigned int cubemapTexture;
    unsigned int texture2D;
    unsigned int heightTexture;
    unsigned int VAO, VBO, EBO;
    int numberOfVerticesToDraw;
    Shader* shader;
    Cubesphere(int subdivision, Shader* shader, int useCubeTexture) {
        this->shader = shader;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        numberOfVerticesToDraw = generateCubeSphereVertices(subdivision);

        shader->use();
        shader->setInt("useTexture", useCubeTexture);
        shader->setInt("textureMap", 0);
        shader->setInt("textureCubeMap", 1);
        shader->setInt("heightMap", 2);
        
        
    }
    void initEarthTextureCubeMap(std::vector<std::string> faces) {
        glGenTextures(1, &cubemapTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        unsigned char* data;

        for (unsigned int i = 0; i < faces.size(); i++)
        {
            data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void initEarthTexture2D() {
        glGenTextures(1, &texture2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2D);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char* data = stbi_load("earth.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

    void initEarthHeightTexture() {
        glGenTextures(1, &heightTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, heightTexture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char* data = stbi_load("heightMap.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

private:
    int generateCubeSphereVertices(int subdivision) {

        int numOfFaces = 6;

        const float DEG2RAD = acos(-1) / 180.0f;
        const float pi = atan(1) * 4;
        glm::vec3 n1;        // normal of longitudinal plane
        glm::vec3 n2;        // normal of latitudinal plane
        glm::vec3 v;         // direction vector intersecting 2 planes, n1 x n2
        float a1;           // longitudinal angle
        float a2;           // latitudinal angle

        // compute the number of vertices per row, 2^n + 1
        int verticesPerRow = (int)pow(2, subdivision) + 1;
        float* vertices = new float[verticesPerRow * verticesPerRow * 5 * numOfFaces];
        unsigned int* indices = new unsigned int[(verticesPerRow - 1) * (verticesPerRow - 1) * 2 * 3 * numOfFaces];
        int vertexCounter = 0;
        int indexCounter = 0;
        for (int f = 0; f < numOfFaces; f++) {
            // rotate latitudinal plane from 45 to -45 degrees
            for (unsigned int i = 0; i < verticesPerRow; ++i)
            {
                // normal for latitudinal plane
                // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
                // therefore, it is rotating (0,1,0) vector by latitude angle a2
                a2 = DEG2RAD * (45.0f - 90.0f * i / (verticesPerRow - 1));
                n2[0] = -sin(a2);
                n2[1] = cos(a2);
                n2[2] = 0;


                // rotate longitudinal plane from -45 to 45
                for (unsigned int j = 0; j < verticesPerRow; ++j)
                {
                    // normal for longitudinal plane
                    // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
                    // therefore, it is rotating (0,0,-1) vector by longitude angle a1
                    a1 = DEG2RAD * (-45.0f + 90.0f * j / (verticesPerRow - 1));
                    n1[0] = -sin(a1);
                    n1[1] = 0;
                    n1[2] = -cos(a1);


                    // find direction vector of intersected line, n1 x n2 and normalize direction vector
                    v = glm::normalize(glm::cross(n1, n2));

                    // add a vertex into array (x,y,z,s,t)
                    float x, y, z;
                    x = calculateVertexCoord(v, (CubeFace)f, 0);
                    y = calculateVertexCoord(v, (CubeFace)f, 1);
                    z = calculateVertexCoord(v, (CubeFace)f, 2);
                    float theta = atan2(-z, x);
                    float phi = acos(-y);
                    vertices[vertexCounter++] = x;
                    vertices[vertexCounter++] = y;
                    vertices[vertexCounter++] = z;
                    vertices[vertexCounter++] = (theta + pi) / (2 * pi);
                    vertices[vertexCounter++] = phi / pi;

                    // generate indices for element buffer
                    if (j < verticesPerRow - 1 && i < verticesPerRow - 1) {
                        indices[indexCounter++] = j + verticesPerRow * i + f * verticesPerRow * verticesPerRow;
                        indices[indexCounter++] = j + 1 + verticesPerRow * i + f * verticesPerRow * verticesPerRow;
                        indices[indexCounter++] = j + verticesPerRow * (i + 1) + f * verticesPerRow * verticesPerRow;

                        indices[indexCounter++] = j + 1 + verticesPerRow * i + f * verticesPerRow * verticesPerRow;
                        indices[indexCounter++] = j + verticesPerRow * (i + 1) + f * verticesPerRow * verticesPerRow;
                        indices[indexCounter++] = j + verticesPerRow * (i + 1) + 1 + f * verticesPerRow * verticesPerRow;
                    }
                }
            }

        }

        glBufferData(GL_ARRAY_BUFFER, vertexCounter * sizeof(float), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCounter * sizeof(float), indices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        delete[] vertices;
        delete[] indices;
        return indexCounter;
    }

    float calculateVertexCoord(glm::vec3 vec, CubeFace face, int axis) {
        switch (face) {
        case POSX:
            //x coord
            if (axis == 0) {
                return vec[0];
            }
            //y coord
            else if (axis == 1) {
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
};



#endif
