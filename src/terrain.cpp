#include "terrain.h"

// Terrain dimensions:
//
//            height
//            _|_
//        ---|   |-----------
//       /  /     \        /   h
//      /  |       |      /   t
//     /                 /   p
//    /                 /   e
//   /                 /   d
//   ------------------
//        width

Terrain::Terrain(GLuint shader_program, std::string heightmap_filename, float amplification)
    : Drawable() {
    // This is where generate the new mesh and override the one passed in by
    // the constructor. This is to save space in the game files, so we don't have a terrain mesh

    // After loading in the heightmap to memory, we can make a terrain mesh
    // based on the data
    float start_time = glfwGetTime();
    mesh = generateMesh(heightmap_filename, amplification);
    float delta_time = glfwGetTime() - start_time;
    Debug::info("Took %f seconds to generate the terrain mesh.\n", delta_time);

    // Once we have a mesh, we can load the drawable data required for this
    // child class.
    Drawable::load(mesh, shader_program, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

    // Could do bit-packing here but it really doesn't matter
    pathing_array = new bool*[depth];
    for(int i = 0; i < depth; ++i){
        pathing_array[i] = new bool[width];
    }

    // Iterate through the pathing array, filling in all the places where we can't go
    for(int z = 0; z < depth; ++z){
        for(int x = 0; x < width; ++x){
            pathing_array[z][x] = (getSteepness(GLfloat(x) + start_x, GLfloat(z) + start_z) < 0.8f);
        }
    }

    // Debugging the allowed areas
    // printPathing();
}

bool Terrain::canPath(int x, int z){
    x -= int(start_x);
    z -= int(start_z);

    if(x < 0 || z < 0 || x > width - 1 || z > depth - 1){
        return false;
    }

    return pathing_array[z][x];
}

void Terrain::printPathing(){
    Debug::info("Pathing array:\n");
    for (int x = 0; x < width - 1; ++x){
        for (int z = 0; z < depth - 1; ++ z){
            printf(" %d", pathing_array[z][x]);
        }
        printf("\n");
    }
}

GLfloat Terrain::getHeight(GLfloat x_pos, GLfloat z_pos){
    // Returns the map height for a specified x and z position.
    // This will be useful for moving units around the terrain.

    // Just do an integer conversion to get the vertex index.
    int x = x_pos;
    int z = z_pos;

    // Offset the positions by the starting positions of the mesh
    x = x - start_x;
    z = z - start_z;

    // Calculate the index of the current point in the vertex array.
    int i = getIndex(x, z);

    if (i < 0 || i > vertices.size() - 1){
        return 0.0f;
    } else {
        return vertices[i].position.y;
    }
}

GLfloat Terrain::getHeightInterpolated(GLfloat x_pos, GLfloat z_pos){
    // Interpolates in the current square. Because the positions get floored
    // We can say that the unit is always "at" the top left.

    //        o--------o
    //        |1      2|
    //        |        |
    //        |3      4|
    //        o--------o
    //     x
    //   o-->
    // z |
    //   v

    // Get the heights for each point
    GLfloat height_1 = getHeight(x_pos, z_pos);
    GLfloat height_2 = getHeight(x_pos + 1, z_pos);
    GLfloat height_3 = getHeight(x_pos, z_pos + 1);

    // Get the change in height from point 2 to 1 and from 3 to 1
    GLfloat delta_height_2 = height_2 - height_1;
    GLfloat delta_height_3 = height_3 - height_1;

    // Find the fractional component of the
    // x and z position to know how much to
    // weight each height.
    double intpart;
    GLfloat x_mult = modf(x_pos, &intpart);
    GLfloat z_mult = modf(z_pos, &intpart);

    // Calculate the height by adding the initial height to the
    // weighted combination of the other two points.
    GLfloat interpolated_height = height_1 + (x_mult * delta_height_2) +
        (z_mult * delta_height_3);

    return interpolated_height;
}

glm::vec3 Terrain::getNormal(GLfloat x_pos, GLfloat z_pos){
    // Returns the normal vector at the specified x and y position.
    // This is good for knowing how a unit can move across a segment
    // of terrain. For example, if the normal is too steep, the unit
    // won't be able to move on that segment.
    // Just do an integer conversion to get the vertex index.
    // Later this should be interpolated using the normal.
    int x = x_pos;
    int z = z_pos;

    // Offset the positions by the starting positions of the mesh
    x = x - start_x;
    z = z - start_z;

    // Calculate the index of the current point in the vertex array.
    int i = getIndex(x, z);

    if (i < 0 || i > vertices.size() - 1){
        return glm::vec3(0.0f, 0.0f, 0.0f);
    } else {
        return vertices[i].normal;
    }
}

GLfloat Terrain::getSteepness(GLfloat x_pos, GLfloat z_pos){
    glm::vec3 normal = getNormal(x_pos, z_pos);
    GLfloat cosTheta = glm::dot(glm::vec3(0.0, 1.0, 0.0), normal);

    GLfloat steepness = acos(cosTheta);

    return steepness;
}

bool Terrain::isOnTerrain(GLfloat x_pos, GLfloat z_pos, GLfloat tolerance){
    bool is_on_terrain = (x_pos >= (-getWidth() / 2.0) + tolerance) &&
        (x_pos <= (getWidth() / 2.0) - tolerance) &&
        (z_pos >= (-getDepth() / 2.0) + tolerance) &&
        (z_pos <= (getDepth() / 2.0) - tolerance);
    return is_on_terrain;
}

void Terrain::initializeBaseMesh(Heightmap& heightmap){
    // This generates the mesh that will be used for the
    // top level terrain data, like height and normals.
    // This is just the basic layout of the mesh though
    // because the actual mesh that gets drawn needs to
    // have more accurate normals and texture coordinates
    // for actually drawing things.
    vertices = std::vector<Vertex>(width * depth);
    for (int x = 0; x < width; ++x){
        for (int z = 0; z < depth; ++z){
            Vertex current;
            float height = heightmap.getMapHeight(x, z);
            current.position = glm::vec3(x + start_x, height, z + start_z);

            int index = getIndex(x, z);
            vertices[index] = current;
        }
    }

    // Make the edge loops to simplify the normal
    // calculations.
    std::vector<GLuint> faces;
    for (int x = 0; x < width - 1; ++x){
        for (int z = 0; z < depth - 1; ++z){
            faces.push_back(getIndex(x, z));
            faces.push_back(getIndex(x + 1, z));
            faces.push_back(getIndex(x, z + 1));

            faces.push_back(getIndex(x + 1, z));
            faces.push_back(getIndex(x + 1, z + 1));
            faces.push_back(getIndex(x, z + 1));
        }
    }

    // Dumb normal calculations without hard edge detection
    // These are sufficient for gameplay terrain data (pathing).
    for (int i = 0; i < faces.size(); i += 3){
        Vertex* a = &vertices[faces[i]];
        Vertex* b = &vertices[faces[i + 1]];
        Vertex* c = &vertices[faces[i + 2]];

        glm::vec3 edge1 = b->position - a->position;
        glm::vec3 edge2 = c->position - a->position;

        glm::vec3 normal = glm::cross(edge2, edge1);

        a->normal += normal;
        b->normal += normal;
        c->normal += normal;

    }

    // Normalize the normals
    for (int i = 0; i < vertices.size(); ++i){
        Vertex* current = &vertices[i];
        current->normal = glm::normalize(current->normal);
    }
}

Mesh* Terrain::generateMesh(std::string filename, float amplification){
    Heightmap heightmap = Heightmap(filename, amplification);

    width = heightmap.getWidth();
    depth = heightmap.getHeight();

    start_x = -width / 2;
    start_z = -depth / 2;

    // Generate the mesh for gameplay data
    initializeBaseMesh(heightmap);

    // Now make it look nice!

    // The number of terrain tiles before the
    // texture repeats.
    int texture_size = 8;

    std::vector<GLuint> faces;
    std::vector<Vertex> textured_vertices;
    for (int x = 0; x < width; x += (texture_size - 1)){
        for (int z = 0; z < depth; z += (texture_size - 1)){

            int start_index = textured_vertices.size();

            // Create the big tile
            for (int i = 0; i < texture_size; ++i){
                for (int j = 0; j < texture_size; ++j){
                    int index = getIndex(x + i, z + j);
                    float u = i / (float)(texture_size - 1);
                    float v = j / (float)(texture_size - 1);

                    Vertex current = vertices[index];
                    current.texcoord = glm::vec2(u, v);
                    textured_vertices.push_back(current);

                }
            }

            for (int i = 0; i < texture_size - 1; ++i){
                for (int j = 0; j < texture_size - 1; ++j){
                    int block_width = texture_size;
                    faces.push_back(start_index + getIndex(i, j, block_width));
                    faces.push_back(start_index + getIndex(i + 1, j, block_width));
                    faces.push_back(start_index + getIndex(i, j + 1, block_width));

                    faces.push_back(start_index + getIndex(i + 1, j, block_width));
                    faces.push_back(start_index + getIndex(i + 1, j + 1, block_width));
                    faces.push_back(start_index + getIndex(i, j + 1, block_width));

                }
            }

        }
    }

    std::vector<GLfloat> out_vertices;
    for (int i = 0; i < textured_vertices.size(); ++i){
        Vertex current = textured_vertices[i];
        out_vertices.push_back(current.position.x);
        out_vertices.push_back(current.position.y);
        out_vertices.push_back(current.position.z);
        out_vertices.push_back(current.normal.x);
        out_vertices.push_back(current.normal.y);
        out_vertices.push_back(current.normal.z);
        out_vertices.push_back(current.tangent.x);
        out_vertices.push_back(current.tangent.y);
        out_vertices.push_back(current.tangent.z);
        out_vertices.push_back(current.binormal.x);
        out_vertices.push_back(current.binormal.y);
        out_vertices.push_back(current.binormal.z);
        out_vertices.push_back(current.texcoord.x);
        out_vertices.push_back(current.texcoord.y);
    }
    return new Mesh(out_vertices, faces);
}

int Terrain::getIndex(int x, int z){
    // For a given x,z coordinate this will return the
    // index for that element in our linear arrays: vertices,
    // and normals.
    return x + ((width) * z);
}

int Terrain::getIndex(int x, int z, int width){
    // For a given x,z coordinate this will return the
    // index for that element in our linear arrays: vertices,
    // and normals.
    return x + ((width) * z);
}

void Terrain::updateUniformData(){
    // Set the scale, this is not really going to be a thing, probably
    // ^ It's definitely a thing
    glUniform1f(glGetUniformLocation(shader_program, "scale"), scale);

}
