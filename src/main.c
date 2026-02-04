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
    GLFWwindow* window = glfwCreateWindow(BOX_LENGTH, BOX_LENGTH, "GLFW Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeLimits(window, BOX_LENGTH, BOX_LENGTH,
                        GLFW_DONT_CARE, GLFW_DONT_CARE);
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

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    RenderQueue queue = {0};
    init_render_queue(&queue, 10, (SizeInt){fbWidth, fbHeight}); // temporarily 10 commands
    Rectangle rect = {.x = 0, .y = 0, .width = fbWidth,
        .height = fbHeight, .color = (Color) {255, 255, 255}, false};
    add_rect_to_queue(&queue, rect, shaders.rectShaderProgram);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen (optional but common)
        glClear(GL_COLOR_BUFFER_BIT);

        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        fbWidth /= 2;
        fbHeight /= 2;
        render(&queue, (SizeInt) {fbWidth, fbHeight});
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
