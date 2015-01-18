// Drawable:
//      The Drawable class is a parent class for any object that needs to be rendered in our 3D world.
//      It has a position, rotation (local), a shader, and a pointer to a Mesh (geometry data).

#include "drawable.h"

Drawable::Drawable(Mesh* mesh, GLuint shader_program){
    load(mesh, shader_program, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
}

Drawable::Drawable(Mesh* mesh, GLuint shader_program, glm::vec3 position, GLfloat scale) {
    load(mesh, shader_program, position, glm::vec3(0.0f, 0.0f, 0.0f), scale);
}

Drawable::Drawable(Mesh* mesh, GLuint shader_program, glm::vec3 position, glm::vec3 rotation, GLfloat scale) {
    load(mesh, shader_program, position, rotation, scale);

}

void Drawable::load(Mesh* mesh, GLuint shader_program, glm::vec3 position, glm::vec3 rotation, GLfloat scale) {
    // Set the position, rotation, scale, and mesh pointer
    this->position = position;
    this->rotation = rotation;
    this->mesh = mesh;
    this->scale = scale;

    // Setup the textures
    // Get the default textures for the drawable.
    GLuint pink = TextureLoader::loadPink();
    GLuint alpha = TextureLoader::loadAlpha();
    GLuint blue = TextureLoader::loadBlue();

    // Setup the drawable such that if no textures
    // are attached later, then it will be bright pink
    this->diffuse = alpha;
    this->specular = alpha;
    this->emissive = pink;
    this->normal = blue;
    this->gloss = alpha;


    // Set the shader program and load the geometry data
    // from the mesh onto it.
    setShader(shader_program);

}

void Drawable::setShader(GLuint shader_program){
    this->shader_program = shader_program;
    this->mesh->attachGeometryToShader(shader_program);

    #warning Global uniform bindings should only ever be called once for each shader
    GLint global_matrix_location = glGetUniformBlockIndex(shader_program, "GlobalMatrices");
    glUniformBlockBinding(shader_program, global_matrix_location, 1);

    GLint shadow_matrix_location = glGetUniformBlockIndex(shader_program, "ShadowMatrices");
    glUniformBlockBinding(shader_program, shadow_matrix_location, 2);

    GLint mouse_point_location = glGetUniformBlockIndex(shader_program, "Mouse");
    glUniformBlockBinding(shader_program, mouse_point_location, 3);

    // Try to set the texture locations
    glUniform1i(glGetUniformLocation(shader_program, "diffuse_texture"), 0);
    glUniform1i(glGetUniformLocation(shader_program, "specular_texture"), 1);
    glUniform1i(glGetUniformLocation(shader_program, "emissive_texture"), 2);
    glUniform1i(glGetUniformLocation(shader_program, "normal_map"), 3);
    glUniform1i(glGetUniformLocation(shader_program, "shadow_map"), 4);


}

void Drawable::setDiffuse(GLuint diffuse) {
    if (diffuse != 0){
        if (emissive == TextureLoader::loadPink()){
            // If this diffuse is being set right after instantiation
            // then the pink emissive would still override. So if the
            // emissive is pink, set it to alpha.
            this->emissive = TextureLoader::loadAlpha();
        }
        this->diffuse = diffuse;
    } else {
        this->diffuse = TextureLoader::loadAlpha();
    }
}

void Drawable::setSpecular(GLuint specular) {
    if (specular != 0){
        this->specular = specular;
    } else {
        this->specular = TextureLoader::loadAlpha();
    }
}
void Drawable::setEmissive(GLuint emissive) {
    if (emissive != 0){
        this->emissive = emissive;
    } else {
        this->emissive = TextureLoader::loadAlpha();
    }
}
void Drawable::setNormal(GLuint normal) {
    if (normal != 0){
        this->normal = normal;
    } else {
        this->normal = TextureLoader::loadAlpha();
    }
}


void Drawable::bindTextures(){
    // Put each texture into the correct location for this Drawable. GL_TEXTURE0-3
    // correspond to the uniforms set in attachTextureSet(). This is where we actually
    // tell the graphics card which textures to use.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissive);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normal);

}

void Drawable::draw(){
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shader_program);

    // Bind the Mesh's VAO. This lets us put transformations and textures on
    // top of the geometry.
    mesh->bindVAO();

    // We need to update the model matrix to account for any rotations
    // or translations that have occured since the last draw call.
    updateModelMatrix();

    // Update the current model, view, and projection matrices in the shader. These are standard for all
    // Drawables so they should always be updated in draw. Child specific data is updated in updateUniformData().
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));

    // Update other shader data
    updateUniformData();

    // Make sure to use this Drawable's textures
    bindTextures();

    // Draw the geometry
    mesh->draw();
}

void Drawable::updateModelMatrix(){
    // Creates the model matrix from the Drawables position and rotation.
    glm::mat4 translation_matrix = glm::translate(glm::mat4(), position);

    // Axes on which to preform the rotations.
    glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 y_axis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 z_axis = glm::vec3(0.0f, 0.0f, 1.0f);

    // Rotate the model about each axis.
    glm::mat4 rotation_matrix;
    rotation_matrix = glm::rotate(rotation_matrix, rotation.x, x_axis);
    rotation_matrix = glm::rotate(rotation_matrix, rotation.y, y_axis);
    rotation_matrix = glm::rotate(rotation_matrix, rotation.z, z_axis);

    model_matrix = rotation_matrix;
    model_matrix = translation_matrix * rotation_matrix;
}
