// ui_window.cpp
// Trevor Westphal

#include "ui_window.h"

UIImage* UIWindow::up_edge_image;
UIImage* UIWindow::right_edge_image;
UIImage* UIWindow::left_edge_image;
UIImage* UIWindow::down_edge_image;

UIImage* UIWindow::up_left_corner_image;
UIImage* UIWindow::up_right_corner_image;
UIImage* UIWindow::down_left_corner_image;
UIImage* UIWindow::down_right_corner_image;

UIWindow::UIWindow(GLuint shader_program) : UIDrawable(shader_program, TextureLoader::loadGray()){
    this->shader = shader_program;
    is_showing = false;

    up_edge_image = 0;
    right_edge_image = 0;
    left_edge_image = 0;
    down_edge_image = 0;

    up_left_corner_image = 0;
    up_right_corner_image = 0;
    down_left_corner_image = 0;
    down_right_corner_image = 0;
}

void UIWindow::loadFromXML(std::string filepath){
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    Debug::info("Parsed layout: %s\n", filepath.c_str());

    pugi::xml_node layout_node = doc.child("layout");

    if(strcmp(layout_node.child_value("type"), "ui_window") != 0){
        Debug::error("Mismatched layout types. Expected ui_window, received %s\n", layout_node.child_value("type"));
        return;
    }

    pugi::xml_node constraints_node = layout_node.child("constraints");
    pugi::xml_node corner_sprites_node = layout_node.child("corner_sprites");
    pugi::xml_node edge_sprites_node = layout_node.child("edge_sprites");
    pugi::xml_node subelements_node = layout_node.child("subelements");

    // Position and Sizing the Window
    parseConstraints(constraints_node);

    // Creating the edges
    if(up_edge_image == NULL){
        up_edge_image = new UIImage(shader, TextureLoader::loadTextureFromFile(edge_sprites_node.child("up").child_value("image_path"), GL_NEAREST));
    }

    if(down_edge_image == NULL){
        down_edge_image = new UIImage(shader, TextureLoader::loadTextureFromFile(edge_sprites_node.child("down").child_value("image_path"), GL_NEAREST));
    }

    if(right_edge_image == NULL){
        right_edge_image = new UIImage(shader, TextureLoader::loadTextureFromFile(edge_sprites_node.child("right").child_value("image_path"), GL_NEAREST));
    }

    if(left_edge_image == NULL){
        left_edge_image = new UIImage(shader, TextureLoader::loadTextureFromFile(edge_sprites_node.child("left").child_value("image_path"), GL_NEAREST));
    }

    // Creating and Positioning the corners
    if(up_left_corner_image == NULL){
        up_left_corner_image = new UIImage(shader, TextureLoader::loadTextureFromFile(corner_sprites_node.child("up_left").child_value("image_path"), GL_NEAREST));
    }

    if(up_right_corner_image == NULL){
        up_right_corner_image = new UIImage(shader, TextureLoader::loadTextureFromFile(corner_sprites_node.child("up_right").child_value("image_path"), GL_NEAREST));
    }

    if(down_right_corner_image == NULL){
        down_right_corner_image = new UIImage(shader, TextureLoader::loadTextureFromFile(corner_sprites_node.child("down_right").child_value("image_path"), GL_NEAREST));
    }

    if(down_left_corner_image == NULL){
        down_left_corner_image = new UIImage(shader, TextureLoader::loadTextureFromFile(corner_sprites_node.child("down_left").child_value("image_path"), GL_NEAREST));
    }

    //###############################################
    // Setting the edge offsets and height/width
    //###############################################
    up_edge_offset_x = atoi(edge_sprites_node.child("up").child_value("x_offset"));
    right_edge_offset_x = atoi(edge_sprites_node.child("right").child_value("x_offset"));
    left_edge_offset_x = atoi(edge_sprites_node.child("left").child_value("x_offset"));
    down_edge_offset_x = atoi(edge_sprites_node.child("down").child_value("x_offset"));

    up_edge_offset_y = atoi(edge_sprites_node.child("up").child_value("y_offset"));
    right_edge_offset_y = atoi(edge_sprites_node.child("right").child_value("y_offset"));
    left_edge_offset_y = atoi(edge_sprites_node.child("left").child_value("y_offset"));
    down_edge_offset_y = atoi(edge_sprites_node.child("down").child_value("y_offset"));

    // Don't need up & down edge width, will be generated by the size of the window
    int right_edge_width = atoi(edge_sprites_node.child("right").child_value("width"));
    int left_edge_width = atoi(edge_sprites_node.child("left").child_value("width"));

    // Don't need right & left edge height, will be generated by the size of the window
    int up_edge_height = atoi(edge_sprites_node.child("up").child_value("height"));
    int down_edge_height = atoi(edge_sprites_node.child("down").child_value("height"));

    //###############################################
    // Setting the corner offsets and height/width
    //###############################################
    up_left_corner_offset_x = atoi(corner_sprites_node.child("up_left").child_value("x_offset"));
    up_right_corner_offset_x = atoi(corner_sprites_node.child("up_right").child_value("x_offset"));
    down_left_corner_offset_x = atoi(corner_sprites_node.child("down_left").child_value("x_offset"));
    down_right_corner_offset_x = atoi(corner_sprites_node.child("down_right").child_value("x_offset"));

    up_left_corner_offset_y = atoi(corner_sprites_node.child("up_left").child_value("y_offset"));
    up_right_corner_offset_y = atoi(corner_sprites_node.child("up_right").child_value("y_offset"));
    down_left_corner_offset_y = atoi(corner_sprites_node.child("down_left").child_value("y_offset"));
    down_right_corner_offset_y = atoi(corner_sprites_node.child("down_right").child_value("y_offset"));

    up_left_corner_width = atoi(corner_sprites_node.child("up_left").child_value("width"));
    up_right_corner_width = atoi(corner_sprites_node.child("up_right").child_value("width"));
    down_left_corner_width = atoi(corner_sprites_node.child("down_left").child_value("width"));
    down_right_corner_width = atoi(corner_sprites_node.child("down_right").child_value("width"));

    up_left_corner_height = atoi(corner_sprites_node.child("up_left").child_value("height"));
    up_right_corner_height = atoi(corner_sprites_node.child("up_right").child_value("height"));
    down_left_corner_height = atoi(corner_sprites_node.child("down_left").child_value("height"));
    down_right_corner_height = atoi(corner_sprites_node.child("down_right").child_value("height"));


    for (pugi::xml_node_iterator it = subelements_node.begin(); it != subelements_node.end(); ++it){
         // printf("Named subelement: %s\n", it->name());

        UIDrawable* ui_element = 0;


        if(strcmp(it->name(), "ui_radiobutton") == 0){
            ui_element = new UIRadioButton(shader);
        } else if(strcmp(it->name(), "ui_button") == 0){
            ui_element = new UIButton(shader);
        }
        
        if(ui_element){
            ui_element->setParent(this);
            ui_element->loadFromXML(it->child_value("layout_filepath"));
            subelements.push_back(ui_element);
        }
    }

    // Convert all pixel coords into screen
    updateDimensions();
    setGLPosition(getGLPosition());
}

void UIWindow::draw(){
    if(is_showing){
        Mouse::getInstance()->setHovering();

        FlatDrawable::draw();

        // int up_edge_offset_x;
        // int right_edge_offset_x;
        // int left_edge_offset_x;
        // int down_edge_offset_x;

        // int up_edge_offset_y;
        // int right_edge_offset_y;
        // int left_edge_offset_y;
        // int down_edge_offset_y;

        // int up_edge_width;
        // int right_edge_width;
        // int left_edge_width;
        // int down_edge_width;

        // int up_edge_height;
        // int right_edge_height;
        // int left_edge_height;
        // int down_edge_height;

        up_edge_image->setPositionAndDimensions(x_pixels, y_pixels - 8, width_pixels, 18);
        right_edge_image->setPositionAndDimensions(x_pixels + width_pixels - 10, y_pixels, 18, height_pixels);
        left_edge_image->setPositionAndDimensions(x_pixels - 8, y_pixels, 18, height_pixels);
        down_edge_image->setPositionAndDimensions(x_pixels, y_pixels + height_pixels - 10, width_pixels, 18);

        up_edge_image->draw();
        right_edge_image->draw();
        left_edge_image->draw();
        down_edge_image->draw();

        up_left_corner_image->setPositionAndDimensions(x_pixels - 17, y_pixels - 17, 62, 62);
        up_right_corner_image->setPositionAndDimensions(x_pixels + width_pixels - 45, y_pixels - 17, 62, 62);
        down_left_corner_image->setPositionAndDimensions(x_pixels - 17, y_pixels + height_pixels - 45, 62, 62);
        down_right_corner_image->setPositionAndDimensions(x_pixels + width_pixels - 45, y_pixels + height_pixels - 45, 62, 62);

        up_left_corner_image->draw();
        up_right_corner_image->draw();
        down_left_corner_image->draw();
        down_right_corner_image->draw();

        for(int i(0); i < subelements.size(); ++i){
            subelements[i]->draw();
        }
    }
}

void UIWindow::show(){
    is_showing = true;
}

void UIWindow::hide(){
    is_showing = false;
}

