#include "game_clock.hpp"

GameClock* GameClock::instance;

GameClock::GameClock(){
    current_time = last_time = getCurrentTime();
    tick_count = 0;
    average_delta_time = 0.0f;

}

GameClock* GameClock::getInstance(){
    if(instance){
        return instance;
    } else {
        instance = new GameClock();
        return instance;
    }
}

void GameClock::tick(){
    ++tick_count;

    last_time = current_time;
    current_time = getCurrentTime();
    delta_time = current_time - last_time;

    // Calculate the moving average of the frame time
    average_delta_time = (average_delta_time *
        (float)(tick_count-1)/(float)tick_count)
        + (delta_time / (float)tick_count);
}

void GameClock::resetAverage(){
    tick_count = 1;
    average_delta_time = 0;
}

float GameClock::getCurrentTime(){
    // Get the number of milliseconds since the SDL library initialization.
    unsigned int ticks = SDL_GetTicks();
    // Convert into seconds
    float seconds = (float) ticks / 1000;
    return seconds;
}

float GameClock::getDeltaTime(){
    return delta_time;
}

float GameClock::getAverageDeltaTime(){
    return average_delta_time;
}
