//
// Created by Omeed on 2/3/26.
//

#include "renderer.h"

#include "../utils/errorhandler.h"

#define SAFE_FREE(p) do { free(p); p = NULL; } while(0)

void init_render_queue(RenderQueue* q, size_t capacity) {
    if (q->commands) { // if commands is not null (there are active commands), then first clear it
        SAFE_FREE(q->commands);
        q->size = 0;
        q->capacity = capacity;
    }
    q->commands = malloc(sizeof(RenderQueue) * capacity);
    if (!q->commands) {
        // if it's somehow null
        throw_exception(MEMORY_ALLOC_ERROR, "Failed to allocate memory for render queue!");
        return;
    }
    q->size = 0; // nothing inside yet
    q->capacity = capacity; // you know the size of the malloc
}

void push_render_queue(RenderQueue* q, RenderCommand command) {
    if (q->capacity == 0) {
        throw_exception(MEMORY_ALLOC_ERROR, "empty capacity!");
        return;
    }
    if (q->commands) {
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

void render(RenderQueue* q) {
    if (!q->commands) {
        throw_exception(NULL_POINTER, "commands is null!");
        return;
    }
    for (int i = 0; i < q->size; i++) {
        RenderCommand* cmd = render_queue_get(q, i);
        if (!cmd) {
            throw_exception(NULL_POINTER, "command is NULL");
            return;
        }
        glUseProgram(cmd->shaderProgram);
        // Bind the vertex array
        glBindVertexArray(cmd->VAO);
        // Draw elements
        glDrawElements(GL_TRIANGLES, cmd->indexCount, GL_UNSIGNED_INT, 0);
        // Optional: unbind VAO/shader for cleanliness
        glBindVertexArray(0);
        glUseProgram(0);
    }
}