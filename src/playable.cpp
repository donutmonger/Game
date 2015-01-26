// playable.cpp
// Trevor Westphal

#include "playable.h"

Playable::Playable() : Drawable(){

}

Playable::Playable(Mesh* mesh, GLuint shader_program, glm::vec3 position, GLfloat scale) : Drawable(mesh, shader_program, position, scale) {

	Mesh* selection_ring_mesh = new Mesh("res/models/selection_ring.dae");

	selection_ring = new Doodad(selection_ring_mesh, shader_program, position, 1.0f);
	selection_ring->setEmissive(TextureLoader::loadTextureFromFile("res/textures/selection_ring.png", GL_LINEAR));
    selection_ring->rotateGlobalEuler(M_PI/2.0f, 0.0f, 0.0f);

	selected = false;
    temp_selected = 0;

    #warning Fix the 90* offset bug
    rotateGlobalEuler(M_PI/2.0f, 0.0f, 0.0f);

    // Temporary stuff until XML parsing is ready
    radius = 1.5f;
    speed = 0.05f;

    move_to_position = position;
    current_direction = 0.0f;
}

void Playable::updateUniformData(){
	glUniform1f(glGetUniformLocation(shader_program, "scale"), scale);
}

void Playable::setMovementTarget(glm::vec3 pos){
    move_to_position = pos;

    float x_delta = move_to_position.x - position.x;
    float z_delta = move_to_position.z - position.z;
    float theta = atan2(x_delta, z_delta);

    rotateGlobalEuler(0.0f, theta - current_direction, 0.0f);
    current_direction = theta; 
}

void Playable::addMovementTarget(glm::vec3 pos){
    movement_stack.push(pos);
}

bool Playable::isMoving(){
    float x_delta = abs(move_to_position.x - position.x);
    float z_delta = abs(move_to_position.z - position.z);

    return (sqrt(x_delta*x_delta + z_delta*z_delta) > 0.01);
}

void Playable::update(Terrain* ground, std::vector<Playable*> otherUnits){

    // If THIS is not at it's target position
    if(isMoving()){

        // Calculate where THIS intends to move
        float move_to_x = position.x + sin(current_direction)*speed;
        float move_to_z = position.z + cos(current_direction)*speed;
    
        // Check all the other units
        for(int i = 0; i < otherUnits.size(); ++i){
            // Find the x and z difference between THIS and other unit
            float x_delta = otherUnits[i]->getPosition().x - move_to_x;
            float z_delta = otherUnits[i]->getPosition().z - move_to_z;

            float abs_x_delta = abs(x_delta);
            float abs_z_delta = abs(z_delta);

            // If it's not THIS and if THIS moves too close
            if(otherUnits[i] != this && sqrt(abs_x_delta*abs_x_delta + abs_z_delta*abs_z_delta) < otherUnits[i]->getRadius() + radius){
                // Push the other unit

                // Get the push direction
                float theta = atan2(x_delta, z_delta);

                // Saving the radius
                float other_radius = otherUnits[i]->getRadius();

                // Add the distance to push to the location-to-be of THIS
                float push_to_x = move_to_x + sin(theta)*(other_radius + radius);
                float push_to_z = move_to_z + cos(theta)*(other_radius + radius);

                // Apply the movement to the other unit IF they aren't moving
                if( ! otherUnits[i]->isMoving()){
                    otherUnits[i]->addMovementTarget(otherUnits[i]->getPosition());
                    otherUnits[i]->addMovementTarget(glm::vec3(push_to_x, 0.0f, push_to_z));
                }
                // otherUnits[i]->setPosition(push_to_x, ground->getHeight(push_to_x, push_to_z), push_to_z);
            }
        }

        // Apply all the position changes
        position.x = move_to_x;
        position.z = move_to_z;
        position.y = ground->getHeight(position.x, position.z);
        selection_ring->setPosition(glm::vec3(position.x, position.y + 0.5, position.z));
        // selection_ring->setPosition(glm::vec3(move_to_position.x, position.y + 0.5, move_to_position.z));
        
    } else if(movement_stack.size() > 0){
    // We have more moves to make
        setMovementTarget(movement_stack.top());
        movement_stack.pop();
    }
}


void Playable::draw(){
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    if(selected || temp_selected ){
    	selection_ring->draw();
    }
    

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

void Playable::select(){
    selected = true;
}

void Playable::deSelect(){
    selected = false;
    temp_selected = false;
}

void Playable::tempSelect(){
    temp_selected = true;
}

void Playable::tempDeSelect(){
    temp_selected = false;
}