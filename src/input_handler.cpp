#include "input_handler.hpp"

InputHandler* InputHandler::instance;

InputHandler* InputHandler::getInstance(){
    if (instance){
        return instance;
    } else {
        instance = new InputHandler();
        return instance;
    }
}

InputHandler::InputHandler(){
    // Set the default callback function
    Callback_Type callback_temp = std::bind(&InputHandler::defaultCallback, this, std::placeholders::_1);
    pushCallback(callback_temp);

    State_Callback_Type state_callback_temp = std::bind(&InputHandler::defaultStateCallback, this);
    setStateCallback(state_callback_temp);
}

void InputHandler::pollInputs() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        callbacks.top()(event);
    }

    state_callback();
}

void InputHandler::pushCallback(Callback_Type& callback){
    callbacks.push(callback);
    Debug::info("Pushed callback %p to top of stack.\n", &callbacks.top());
}

void InputHandler::popCallback(){
    Debug::info("Popping callback %p off of the stack.\n", &callbacks.top());
    callbacks.pop();
}

void InputHandler::setStateCallback(State_Callback_Type state_callback){
    this->state_callback = state_callback;
}

void InputHandler::defaultCallback(SDL_Event event){

}

void InputHandler::defaultStateCallback(){

}
