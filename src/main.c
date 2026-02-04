#define GL_SILENCE_DEPRECATION
#include "graphics/graphic_libs.h"
#include "graphics/rectangle.h"
#include "graphics/renderer.h"
#include "shader/shader.h"

//#define DEFAULT_WIDTH 800
//#define DEFAULT_HEIGHT 800
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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
    GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "GLFW Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    // Make the window's context current
    glfwMakeContextCurrent(window);

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

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen (optional but common)
        glClear(GL_COLOR_BUFFER_BIT);
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        int len = MIN(fbWidth, fbHeight);
        Rectangle rect = {(fbWidth-len)/2, (fbHeight-len)/2, len, len, false};
        draw_rectangle(rect,
            (SizeInt) {fbWidth, fbHeight}, shaders);
        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
