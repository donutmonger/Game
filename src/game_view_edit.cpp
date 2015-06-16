#include "game_view_edit.h"

GameViewEdit::GameViewEdit(Level& level, RenderDeque& render_stack) : GameView(level, render_stack), placer(level) {


    current_paint = new UIDrawable();
    current_paint->setPixelCoordinates(20, 220, 120, 320);

    fancy_text = new TextRenderer("BreeSerif-Regular.ttf", 18);

    setMode(Painting);

}

void GameViewEdit::update(){
    glm::vec2 mouse_gl_pos = Mouse::getInstance()->getGLPosition();
    glm::vec3 mouse_world_pos = level->getGameMap().calculateWorldPosition(mouse_gl_pos);

    if (current_mode == Placing) {
        placer.update(mouse_world_pos);
    }

    GameView::update();

}

void GameViewEdit::drawOtherStuff(){
    if (current_mode == Painting) {
        drawPaintingUI();
    } else if (current_mode == Placing) {
        drawPlacingUI();
    }
}

void GameViewEdit::drawPaintingUI() {
    current_paint->draw();

    TextureLayer current_layer = level->getGameMap().getGround().getCurrentLayer();
    Texture paint_texture = current_layer.getDiffuse();
    current_paint->attachTexture(paint_texture);
    current_paint->setPixelCoordinates(20, 220, 120, 320);
    fancy_text->print(20, 180, "Paint: LMB");
    fancy_text->print(20, 200, "Erase: RMB");
    fancy_text->print(30, 230, "%d", current_layer.getLayerNumber());
}

void GameViewEdit::drawPlacingUI() {
    fancy_text->print(20, 180, "Placing Mode");

}

void GameViewEdit::handleInputState(){
    GameView::handleMouseCameraMovement();

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // Get the mouse coordinates gl, and the world
    glm::vec2 mouse_gl_pos = Mouse::getInstance()->getGLPosition();
    glm::vec3 mouse_world_pos = level->getGameMap().calculateWorldPosition(mouse_gl_pos);

    if (current_mode == Painting) {
        if (Mouse::getInstance()->isPressed(Mouse::LEFT)){
            level->getGameMap().getGround().paintSplatmap(mouse_world_pos);
        }
        if (Mouse::getInstance()->isPressed(Mouse::RIGHT)){
            level->getGameMap().getGround().eraseSplatmap(mouse_world_pos);
        }

        if (state[SDL_SCANCODE_1]){
            this->level->getGameMap().getGround().setPaintLayer(1);
        } else if (state[SDL_SCANCODE_2]){
            this->level->getGameMap().getGround().setPaintLayer(2);
        } else if (state[SDL_SCANCODE_3]){
            this->level->getGameMap().getGround().setPaintLayer(3);
        } else if (state[SDL_SCANCODE_4]){
            this->level->getGameMap().getGround().setPaintLayer(4);
        } else if (state[SDL_SCANCODE_5]){
            this->level->getGameMap().getGround().setPaintLayer(5);
        } else if (state[SDL_SCANCODE_6]){
            this->level->getGameMap().getGround().setPaintLayer(6);
        }
    }


}

void GameViewEdit::handleInput(SDL_Event event){
    GameView::handleInput(event);

    SDL_Scancode key_scancode = event.key.keysym.scancode;
    switch(event.type){
        case SDL_KEYDOWN:
            if ((key_scancode == SDL_SCANCODE_TAB) && (!tab_key_state)) {
                tab_key_state = true;
                cycleMode();
            }
        break;

        case SDL_KEYUP:
            if (key_scancode == SDL_SCANCODE_TAB) {
                tab_key_state = false;
            }
        break;
    }
}

void GameViewEdit::setMode(Mode mode) {
    current_mode = mode;

    if (mode == Painting) {
        this->level->getGameMap().getGround().setPaintLayer(1);
    } else if (mode == Placing) {
        Drawable& drawable_ref = placer.getDrawable();
        this->level->getGameMap().addDrawable(drawable_ref);
    }
}

void GameViewEdit::cycleMode() {
    if (current_mode == Painting) {
        setMode(Placing);
    } else if (current_mode == Placing) {
        setMode(Painting);
    }
}
