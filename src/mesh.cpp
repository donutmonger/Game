#include "mesh.h"

Mesh::Mesh(const char* filename, GLfloat scale){
    this->scale = scale;

    MeshLoader mesh_loader = MeshLoader(filename);
    GLfloat* vertices = mesh_loader.getVertexArray();
    GLuint* elements = mesh_loader.getFaceArray();
    num_faces = mesh_loader.getFacesSize();

    GLuint vbo, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);               // Generate 1 buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);  // Make vbo the active array buffer
    glBufferData(GL_ARRAY_BUFFER, mesh_loader.getVerticesSize() * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_loader.getFacesSize() * sizeof(GLuint), elements, GL_STATIC_DRAW);

}

void Mesh::draw(glm::mat4* view_matrix, glm::mat4* proj_matrix, glm::mat4* model_matrix, TextureSet* texture_set, Light* light){
    glBindVertexArray(vao);
    
    // Set the scale, this is not really going to be a thing, probably
    glUniform1f(glGetUniformLocation(shader_program, "scale"), this->scale);

    // Update the time uniform
    glUniform1f(glGetUniformLocation(shader_program, "time"), (float)glfwGetTime());

    glUniform3fv(glGetUniformLocation(shader_program, "light.position"), 1, light->getPosition());
    glUniform3fv(glGetUniformLocation(shader_program, "light.color"), 1, light->getColor());
    glUniform1fv(glGetUniformLocation(shader_program, "light.intensity"), 1, light->getIntensity());

    // Update the model, view, and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(*model_matrix));
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(*view_matrix));    
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "proj"), 1, GL_FALSE, glm::value_ptr(*proj_matrix));

    printf("#################\n");
    printf("%d\n", texture_set->diffuse);
    printf("%d\n", texture_set->specular);
    printf("%d\n", texture_set->normal);
    printf("%d\n", texture_set->emissive);
    printf("#################\n");


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_set->diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_set->specular);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_set->normal);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture_set->emissive);

    glDrawElements(GL_TRIANGLES, this->num_faces, GL_UNSIGNED_INT, 0);

}

void Mesh::attachShader(GLuint shader_program){
    this->shader_program = shader_program;

    // Get the reference to the "position" attribute defined in
    // the vertex shader
    GLint posAttrib = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(posAttrib);
    // Load the position attributes from our array with width 3. The position
    // values start at index 0. Tell it to load 2 values
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
                           8*sizeof(float), 0);

    GLint normalAttrib = glGetAttribLocation(shader_program, "normal");
    glEnableVertexAttribArray(normalAttrib);
    // Load the normal pointer from our array with width 3. The normal values
    // start at index 2. Tell it to load 3 value
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE,
                           8*sizeof(float), (void*)(3*sizeof(float)));

    // Link the texture coordinates to the shader.
    GLint texAttrib = glGetAttribLocation(shader_program, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                           8*sizeof(float), (void*)(6*sizeof(float)));

    glUniform1i(glGetUniformLocation(shader_program, "diffuse_texture"), 0);
    glUniform1i(glGetUniformLocation(shader_program, "specular_texture"), 1);
    glUniform1i(glGetUniformLocation(shader_program, "normal_map"), 2);
    glUniform1i(glGetUniformLocation(shader_program, "emissive_texture"), 3);
}