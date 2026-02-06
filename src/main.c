#define GL_SILENCE_DEPRECATION
#include "graphics/graphic_libs.h"
#include "graphics/rectangle.h"
#include "graphics/renderer.h"
#include "shader/shader.h"

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    int window_length = MIN(WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowSizeLimits(window, window_length/2, window_length/2,
                        GLFW_DONT_CARE, GLFW_DONT_CARE);
    // Make the window's context current
    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetWindowAspectRatio(window, 1, 1);

    // 3. Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // Immediately test function pointer
    if (!glGenVertexArrays) {
        fprintf(stderr, "glGenVertexArrays pointer is still NULL!\n");
        return -1;
    }

    ShaderBuffers shaders = init_shaders();

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    fbWidth /= 2;
    fbHeight /= 2;
    RenderQueue queue = {0};
    init_rq(&queue, 10, (size_int){fbWidth, fbHeight}, shaders); // temporarily 10 commands
    add_tiles_to_rq(&queue);

    glfwSetWindowUserPointer(window, &queue);

    Board board = init_board();
    add_board_to_rq(&queue, &board);

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen (optional but common
        // Poll for and process events
        glfwPollEvents();
       // poll_left_click(window, &queue);
        render(&queue, window);
        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
