#include "window.hpp"

Window* Window::instance;

Window::Window(){
    should_close = false;
    //Start SDL
    SDL_Init(SDL_INIT_EVERYTHING);
}

Window* Window::getInstance(){
    if(instance){
        return instance;
    } else {
        instance = new Window();
        return instance;
    }
}

void Window::display(){
    //Update screen
    SDL_GL_SwapWindow(sdl_window);
}

void Window::requestClose() {
    Debug::info("Window close request received.\n");
    should_close = true;
}

bool Window::shouldClose(){
    return should_close;
}


void Window::close(){
    // Quit SDL
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}

void Window::setVsync(bool value){
    if (value){
        SDL_GL_SetSwapInterval(1);
    } else {
        SDL_GL_SetSwapInterval(0);
    }
}

void Window::setFullscreen(bool fullscreen){
    if (fullscreen){
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, 0, &mode);
        width = mode.w;
        height = mode.h;
    }
    this->fullscreen = fullscreen;
}

void Window::setWidth(int width){
    this->width = width;
}

void Window::setHeight(int height){
    this->height = height;
}

int Window::getWidth(){
    return width;
}

int Window::getHeight(){
    return height;
}

void Window::takeScreenshot(){
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y_%m_%d-%H_%M_%S", &tstruct);

    std::string filename = "screenshot_" + std::string(buf) + ".bmp";

    // Create the unsigned byte array of size components * width * height
    // In this case components is 3 because we are saving the RGB components
    int components = 4;
    // Flipped vertically
    GLubyte* data = new GLubyte[components * width * height];
    glReadPixels(0, 0, width, height, GL_RGBA,
        GL_UNSIGNED_BYTE, data);

    // Flip the image about the horizontal axis
    GLubyte* correct_data = new GLubyte[components * width * height];
    int image_width = components * width;
    int image_height = height;
    for (int x = 0; x < image_width; x += components){
        for (int y = 0; y < image_height; ++y){
            for (int c = 0; c < components; ++c){
                int data_index = c + x + ((image_height - y - 1) * image_width);
                int correct_index = c + x + (y * image_width);

                GLubyte this_byte;
                if (c == 3){
                    // Set all alpha components to maximum value (-1)
                    // for unsigned byte
                    this_byte = -1;
                } else{
                    this_byte = data[data_index];
                }

                correct_data[correct_index] = this_byte;
            }
        }
    }

    int save_result = SOIL_save_image(filename.c_str(), SOIL_SAVE_TYPE_BMP,
        width, height, components, correct_data);
    if (save_result){
        Debug::info("Took screenshot %s.\n", filename.c_str());
    } else {
        Debug::error("Error saving screenshot %s.\n", filename.c_str());
    }

    delete[] data;
    delete[] correct_data;
    data = NULL;
    correct_data = NULL;
}

void Window::initializeWindow(){
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    // Use OpenGL 4.1 if possible
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

    if (fullscreen){
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    sdl_window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    gl_context = SDL_GL_CreateContext(sdl_window);

    SDL_ShowCursor(SDL_DISABLE);

    // Set up GLEW so that we can use abstracted OpenGL functions
    glewExperimental = GL_TRUE;
    glewInit();

    // Print various info about OpenGL
    Debug::info("Renderer:       %s\n", glGetString(GL_RENDERER));
    Debug::info("OpenGL version: %s\n", glGetString(GL_VERSION));
    Debug::info("GLSL version:   %s\n",
        glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Set up the correct depth rendering
    glEnable(GL_DEPTH_TEST);

    // Describe what constitutes the front face, and enable backface culling
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    // Alpha transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

}

void Window::centerMouse(){
    SDL_WarpMouseInWindow(sdl_window, width / 2, height / 2);
}
