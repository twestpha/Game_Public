#include "debug_console.hpp"

DebugConsole* DebugConsole::instance;

DebugConsole* DebugConsole::getInstance(){
    if(instance){
        return instance;
    } else {
        instance = new DebugConsole();
        return instance;
    }

}

DebugConsole::DebugConsole() : UIWindow() {
    loadFromXML("res/layouts/debug_console.xml");
    text_renderer = new TextRenderer("Inconsolata-Bold.ttf", 18);

    // Do this so we don't have to process a ton of messages on the first
    // draw call
    syncWithDebug();
    input_buffer = "";
}

void DebugConsole::draw(){
    syncWithDebug();

    if (is_showing){
        UIWindow::draw();
        int start = 0;
        int end = messages.size() - 1;
        if (messages.size() > 15){
            start = end - 15;
        }
        for (int i = start; i <= end; ++i){
            const char* temp_buffer = messages[i].c_str();
            int x = x_pixels + 16;
            int y_base = y_pixels + height_pixels - 32 - (22);
            int y_offset = 22 * (end - i);
            int y = y_base - y_offset;
            text_renderer->print(x, y, temp_buffer);
        }

        // Print the input buffer
        std::string input_prompt = "> " + input_buffer;
        text_renderer->print(x_pixels + 16, y_pixels + height_pixels - 32, input_prompt.c_str());

    }

}

void DebugConsole::show(){
    InputHandler::Callback_Type callback_function = std::bind(&DebugConsole::handleInput, this, std::placeholders::_1);
    InputHandler::getInstance()->pushCallback(callback_function);

    UIWindow::show();
}

void DebugConsole::hide(){
    InputHandler::getInstance()->popCallback();
    UIWindow::hide();
}

void DebugConsole::clearMessages(){
    messages.clear();
}

void DebugConsole::handleInput(SDL_Event event){
    SDL_Scancode key_scancode = event.key.keysym.scancode;
    switch (event.type) {
    case SDL_KEYUP:
        // Debug::info("Released: %s\n", SDL_GetKeyName(event.key.keysym.sym));
        break;
    case SDL_KEYDOWN:
        if (key_scancode == SDL_SCANCODE_ESCAPE){
            Window::getInstance()->requestClose();
        } else if (key_scancode == SDL_SCANCODE_F8){
            hide();
        } else if (key_scancode == SDL_SCANCODE_BACKSPACE){
            if (input_buffer.size() > 0){
                input_buffer.pop_back();
            }
        } else if (key_scancode == SDL_SCANCODE_RETURN){
            if (input_buffer.empty()){
                hide();
            } else {
                parseInput();
                input_buffer.clear();
            }
        }
        break;

    case SDL_TEXTINPUT:
        // Add new text onto the end of our input buffer
        input_buffer += event.text.text;
        break;
    case SDL_QUIT:
        Window::getInstance()->requestClose();
        break;
    }
}

void DebugConsole::syncWithDebug(){
    // Get all of the messages that debug has in its queue
    while (Debug::hasMessages()){
        std::string new_message = Debug::popMessage();
        messages.push_back(new_message);
    }
}

void DebugConsole::parseInput(){
    // Split the string by spaces
    std::string temp_buffer = input_buffer;
    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    std::vector<std::string> tokens;
    while ((pos = temp_buffer.find(delimiter)) != std::string::npos) {
        token = temp_buffer.substr(0, pos);
        tokens.push_back(token);
        temp_buffer.erase(0, pos + delimiter.length());
    }
    tokens.push_back(temp_buffer);

    // See if this is a valid command (beautiful code)
    if (tokens[0] == "set"){
        if (tokens[1] == "layer"){
            try {
                int layer_num = std::stoi(tokens[2]);
                // level->getTerrain()->setPaintLayer(layer_num);
            } catch (std::invalid_argument e){

            }
        }
    } else if (tokens[0] == "swap"){
        if (tokens[1] == "layer"){
            try {
                int layer1 = std::stoi(tokens[2]);
                int layer2 = std::stoi(tokens[3]);
                // level->getTerrain()->getLayeredTextures()->swapLayers(layer1, layer2);
            } catch (std::invalid_argument e){

            }
        }
    } else if (tokens[0] == "save"){
        if (tokens[1] == "layer"){
            if (tokens.size() > 2){
                // std::string output = level->getTerrain()->getLayeredTextures()->saveData(tokens[2]);
                // Debug::info("%s", output.c_str());
            }
        } else {
            if (tokens.size() > 1){
                // std::string output = level->getTerrain()->saveData(tokens[1]);
                // Debug::info("%s", output.c_str());
            }
        }
    } else {
        Debug::error("Unknown command: '%s'\n", input_buffer.c_str());
    }
}
