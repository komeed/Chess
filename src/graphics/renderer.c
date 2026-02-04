//
// Created by Omeed on 2/3/26.
//

#include "renderer.h"

#include "../utils/errorhandler.h"

#define SAFE_FREE(p) do { free(p); p = NULL; } while(0)

void init_render_queue(RenderQueue* q, size_t capacity, SizeInt windowSize) {
    q->windowSize = windowSize;

    SAFE_FREE(q->commands);  // SAFE_FREE should handle NULL

    q->commands = malloc(sizeof(RenderCommand) * capacity);
    if (!q->commands) {
        throw_exception(MEMORY_ALLOC_ERROR,
                        "Failed to allocate memory for render queue!");
        return;
    }

    q->size = 0;
    q->capacity = capacity;
}

void push_render_queue(RenderQueue* q, RenderCommand command) {
    if (q->capacity == 0) {
        throw_exception(MEMORY_ALLOC_ERROR, "empty capacity!");
        return;
    }
    if (!q->commands) {
        throw_exception(MEMORY_ALLOC_ERROR, "commands is null!");
        return;
    }
    if (q->size >= q->capacity) { // if capacity exceeded
        q->capacity *= 2;
        RenderCommand* temp = realloc(q->commands, sizeof(RenderCommand) * q->capacity);
        if (!temp) {
            throw_exception(MEMORY_ALLOC_ERROR, "Failed to allocate memory for render queue!");
            return; // original q->commands is still valid
        }
        q->commands = temp; // only assign if realloc succeeded
    }
    q->commands[q->size] = command;
    q->size++;
}

RenderCommand* render_queue_get(RenderQueue* q, int index) {
    if (!q) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return NULL;
    }
    if (index >= q->size) {
        throw_exception(INDEX_OUT_OF_BOUNDS, "index out of bounds!");
        return NULL;
    }
    return &q->commands[index];
}

void clear_render_queue(RenderQueue* q) {
    q->size = 0;
}

void free_render_queue(RenderQueue* q) {
    SAFE_FREE(q->commands); // safe even if q->commands is NULL
    q->size = 0;
    q->capacity = 0;
}

//assumes that all commands already initialized with vao
void render(RenderQueue* q, SizeInt windowSize) {
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    q->windowSize = windowSize;
    if (!q->commands) {
        throw_exception(NULL_POINTER, "commands is null!");
        return;
    }
    float scaleX = (float)BOX_LENGTH/windowSize.width;
    float scaleY = (float)BOX_LENGTH/windowSize.height;
    for (int i = 0; i < q->size; i++) {
        RenderCommand* cmd = render_queue_get(q, i);
        if (!cmd) {
            throw_exception(NULL_POINTER, "command is NULL");
            return;
        }
        int vertexScaleLocation = glGetUniformLocation(cmd->shaderProgram, "uScale");
        int fragColorLocation   = glGetUniformLocation(cmd->shaderProgram, "uColor");

        glUseProgram(cmd->shaderProgram);
        glUniform3f(fragColorLocation, cmd->color.r/255.0f, cmd->color.g/255.0f, cmd->color.b/255.0f);
        glUniform2f(vertexScaleLocation, scaleX, scaleY);
        // Bind the vertex array
        glBindVertexArray(cmd->VAO);
        // Draw elements
        glDrawElements(GL_TRIANGLES, cmd->indexCount, GL_UNSIGNED_INT, 0);
    }
}

//based on default 800x800 frame (scale appropriately
void add_rect_to_queue(RenderQueue *q, const Rectangle rect, const GLuint shaderProgram) {
    // Convert from pixel coordinates to Normalized Device Coordinates (-1 to 1)
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    float left, right, bottom, top;
    compute_ndc_for_rect(rect, q->windowSize, &left, &right, &bottom, &top);

    printf("Rect vertices: %f %f %f %f %f %f %f %f\n", left,bottom,right,bottom,right,top,left,top);

    float vertices[] = {
        left,  bottom,  // bottom-left
        right, bottom,  // bottom-right
        right, top,     // top-right
        left,  top      // top-left
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };

    unsigned int VAO, VBO, EBO;
    if (!glGenVertexArrays) {
        fprintf(stderr, "glGenVertexArrays is NULL!\n");
        exit(1);
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    RenderCommand command = {.shaderProgram = shaderProgram, .VAO = VAO, .indexCount = 6,
    .color = rect.color};
   push_render_queue(q, command);

    // Draw rectangle
   // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
/* // don't delete these because we're reusing
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);*/
}

void add_rects_to_queue(RenderQueue *q, RectArray *rectArray, GLuint shaderProgram) {
    if (q == NULL) {
        throw_exception(NULL_POINTER, "RenderQueue is NULL");
        return;
    }
    if (rectArray == NULL) {
        throw_exception(NULL_POINTER, "RectArray is NULL");
        return;
    }

    size_t vertices_size = 8 * rectArray->size;
    size_t indices_size = 6 * rectArray->size;
    float* vertices = malloc(sizeof(float) * vertices_size); // 8 vertices for each
    float* indices = malloc(sizeof(float) * indices_size); // 6 indices (2 triangles)

    for (int i = 0; i < rectArray->size; i++) {
        Rectangle rect = rect_array_get(rectArray, i);
        //compute ndc
        float left, right, bottom, top;
        compute_ndc_for_rect(rect, q->windowSize, &left, &right, &bottom, &top);

    }
}