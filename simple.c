#include "simple.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Struct for linked list node of hunks
typedef struct xdiff_hunk_node {
    xdiff_hunk_t hunk;
    struct xdiff_hunk_node *next;
} xdiff_hunk_node_t;

// Internal structure for linked list management
typedef struct xdiff_result_internal {
    xdiff_hunk_node_t *head; // Head of the linked list
    xdiff_hunk_node_t *tail; // Tail of the linked list for fast appending
    size_t hunk_count;       // Number of hunks in the list
} xdiff_result_internal_t;

// Appends a new hunk to the linked list
static xdiff_hunk_t *append_hunk(
  xdiff_result_internal_t *result,
  long old_begin,
  long old_count, long
  new_begin,
  long new_count
) {
    if (!result) return NULL;

    xdiff_hunk_node_t *node = malloc(sizeof(xdiff_hunk_node_t));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed in append_hunk\n");
        return NULL;
    }

    // Initialize the hunk
    node->hunk.old_begin = old_begin;
    node->hunk.old_count = old_count;
    node->hunk.new_begin = new_begin;
    node->hunk.new_count = new_count;
    node->hunk.lines = NULL;
    node->hunk.line_count = 0;
    node->next = NULL;

    // Append to the linked list
    if (result->tail) {
        result->tail->next = node;
    } else {
        result->head = node;
    }
    result->tail = node;
    result->hunk_count++;

    return &node->hunk;
}

// Callback for processing hunks
static int hunk_callback(
  void *priv,
  long old_begin,
  long old_count,
  long new_begin,
  long new_count,
  const char *func,
  long funclen
) {
    xdiff_result_internal_t *result = (xdiff_result_internal_t *)priv;
    if (!append_hunk(result, old_begin, old_count, new_begin, new_count)) {
        return -1; // Indicate failure
    }
    return 0; // Indicate success
}

// Callback for processing lines within a hunk
static int line_callback(
  void *priv,
  mmbuffer_t *line,
  int type
) {
    // Validate input parameters
    if (!priv) {
        fprintf(stderr, "Error: Invalid priv pointer in line_callback\n");
        return -1;
    }

    if (!line || !line->ptr || line->size < 0) {
        fprintf(stderr, "Error: Invalid line data (null or bad size) in line_callback\n");
        return -1;
    }

    xdiff_result_internal_t *result = (xdiff_result_internal_t *)priv;

    // Check if there is a hunk available to associate the line
    if (!result->tail) {
        fprintf(stderr, "Error: No hunk exists to associate the line\n");
        return -1;
    }

    xdiff_hunk_t *current_hunk = &result->tail->hunk;

    // Allocate memory for line content + null terminator
    char *line_content = malloc(line->size + 1);
    if (!line_content) {
        fprintf(stderr, "Error: Memory allocation failed for line content in line_callback\n");
        return -1;
    }

    // Copy line content and add null terminator
    memcpy(line_content, line->ptr, line->size);
    line_content[line->size] = '\0';

    // Optimize reallocation for the hunk's lines array using a growth factor
    size_t new_capacity = current_hunk->line_count == 0 ? 1 : current_hunk->line_count * 2;
    if (current_hunk->line_count + 1 > new_capacity) {
        new_capacity = current_hunk->line_count + 1;
    }
    char **new_lines = realloc(current_hunk->lines, new_capacity * sizeof(char *));
    if (!new_lines) {
        fprintf(stderr, "Error: Memory reallocation failed for lines array in line_callback\n");
        free(line_content); // Clean up already allocated content
        return -1;
    }

    // Update hunk's lines array
    current_hunk->lines = new_lines;
    current_hunk->lines[current_hunk->line_count++] = line_content;

    return 0;
}

// Converts the linked list of hunks into an array, properly copying the lines
static xdiff_hunk_t *finalize_hunks(xdiff_result_internal_t *result) {
    if (!result || result->hunk_count == 0) {
        return NULL;
    }

    // Allocate memory for the array of hunks
    xdiff_hunk_t *hunks = malloc(result->hunk_count * sizeof(xdiff_hunk_t));
    if (!hunks) {
        fprintf(stderr, "Error: Memory allocation failed in finalize_hunks\n");
        return NULL;
    }

    xdiff_hunk_node_t *current = result->head;
    for (size_t i = 0; i < result->hunk_count; i++) {
        if (!current) {
            fprintf(stderr, "Error: Linked list traversal failed in finalize_hunks\n");
            free(hunks);
            return NULL;
        }

        // Copy the hunk metadata (like old_begin, old_count, etc.)
        hunks[i] = current->hunk;
        hunks[i].lines = NULL; // Initialize to NULL before copying lines

        // Allocate memory for copying lines
        if (current->hunk.line_count > 0) {
            hunks[i].lines = malloc(current->hunk.line_count * sizeof(char *));
            if (!hunks[i].lines) {
                fprintf(stderr, "Error: Memory allocation failed for lines array in finalize_hunks\n");

                // Free everything created so far
                for (size_t j = 0; j < i; j++) {
                    for (size_t k = 0; k < hunks[j].line_count; k++) {
                        free(hunks[j].lines[k]);
                    }
                    free(hunks[j].lines);
                }
                free(hunks);
                return NULL;
            }

            // Copy each line into newly allocated memory
            for (size_t j = 0; j < current->hunk.line_count; j++) {
                hunks[i].lines[j] = strdup(current->hunk.lines[j]);
                if (!hunks[i].lines[j]) {
                    fprintf(stderr, "Error: Memory allocation failed for line content in finalize_hunks\n");

                    // Cleanup for current hunk's lines
                    for (size_t k = 0; k < j; k++) {
                        free(hunks[i].lines[k]);
                    }
                    free(hunks[i].lines);

                    // Cleanup for previous hunks
                    for (size_t k = 0; k < i; k++) {
                        for (size_t l = 0; l < hunks[k].line_count; l++) {
                            free(hunks[k].lines[l]);
                        }
                        free(hunks[k].lines);
                    }

                    free(hunks);
                    return NULL;
                }
            }
        }

        // Update line_count
        hunks[i].line_count = current->hunk.line_count;

        // Move to the next node in the linked list
        current = current->next;
    }

    return hunks;
}


// Frees the linked list of hunks
static void free_hunk_list(
  xdiff_result_internal_t *result
) {
    if (!result) return;

    xdiff_hunk_node_t *current = result->head;
    while (current) {
        xdiff_hunk_node_t *next = current->next;
        for (size_t i = 0; i < current->hunk.line_count; i++) {
            free(current->hunk.lines[i]);
        }
        free(current->hunk.lines);
        free(current);
        current = next;
    }
}

// Simplified wrapper for xdl_diff
xdiff_result_t *xdl_xdiff_simple(
  mmfile_t *mf1,
  mmfile_t *mf2,
  xpparam_t *xpp,
  xdemitconf_t *xecfg
) {
    xdiff_result_internal_t internal_result = {0};

    xdemitcb_t ecb = {
        .priv = &internal_result,
        .out_hunk = hunk_callback,
        .out_line = line_callback
    };

    if (xdl_diff(mf1, mf2, xpp, xecfg, &ecb) < 0) {
        free_hunk_list(&internal_result);
        return NULL;
    }

    xdiff_result_t *result = malloc(sizeof(xdiff_result_t));
    if (!result) {
        fprintf(stderr, "Error: Memory allocation failed for xdiff_result_t\n");
        free_hunk_list(&internal_result);
        return NULL;
    }

    result->hunks = finalize_hunks(&internal_result);
    result->hunk_count = internal_result.hunk_count;

    free_hunk_list(&internal_result);
    return result;
}

// Frees the memory allocated for the xdiff_result_t structure
void xdiff_result_destroy(
  xdiff_result_t *result
) {
    if (!result) return;

    for (size_t i = 0; i < result->hunk_count; i++) {
        for (size_t j = 0; j < result->hunks[i].line_count; j++) {
            free(result->hunks[i].lines[j]);
        }
        free(result->hunks[i].lines);
    }

    free(result->hunks);
    free(result);
}

// Accessor for old_begin
long xdiff_hunk_get_old_begin(
  xdiff_hunk_t *hunk
) {
    return hunk ? hunk->old_begin : -1;
}

// Accessor for old_count
long xdiff_hunk_get_old_count(
  xdiff_hunk_t *hunk
) {
    return hunk ? hunk->old_count : -1;
}

// Accessor for new_begin
long xdiff_hunk_get_new_begin(
  xdiff_hunk_t *hunk
) {
    return hunk ? hunk->new_begin : -1;
}

// Accessor for new_count
long xdiff_hunk_get_new_count(
  xdiff_hunk_t *hunk
) {
    return hunk ? hunk->new_count : -1;
}

// Accessor for line_count
size_t xdiff_hunk_get_line_count(
  xdiff_hunk_t *hunk
) {
    return hunk ? hunk->line_count : 0;
}

// Accessor for specific line
const char *xdiff_hunk_get_line_at(
  xdiff_hunk_t *hunk,
  size_t index
) {
    if (hunk && index < hunk->line_count) {
        char* line = hunk->lines[index];
        return line;
    }
    return NULL;
}

// Getter functions for xdiff_result_t
size_t xdiff_result_get_hunk_count(
  xdiff_result_t *result
) {
    return result ? result->hunk_count : 0;
}

xdiff_hunk_t *xdiff_result_get_hunk_at(
  xdiff_result_t *result,
  size_t index
) {
    if (result && index < result->hunk_count) {
        return &result->hunks[index];
    }
    return NULL;
}