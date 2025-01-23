#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdio.h> // For error messages

// mmfile_t functions
mmfile_t *create_mmfile(
  const char *data,
  long size
) {
    mmfile_t *mmfile = (mmfile_t *)malloc(sizeof(mmfile_t));
    if (mmfile) {
        mmfile->ptr = (char *)malloc(size);
        if (mmfile->ptr) {
            memcpy(mmfile->ptr, data, size);
            mmfile->size = size;
        } else {
            free(mmfile);
            return NULL;
        }
    }
    return mmfile;
}

void destroy_mmfile(
  mmfile_t *mmfile
) {
    if (mmfile) {
        free(mmfile->ptr);
        free(mmfile);
    }
}

// mmbuffer_t functions
mmbuffer_t *create_mmbuffer(
  const char *data,
  long size
) {
    mmbuffer_t *mmbuffer = (mmbuffer_t *)malloc(sizeof(mmbuffer_t));
    if (mmbuffer) {
        mmbuffer->ptr = (char *)malloc(size);
        if (mmbuffer->ptr) {
            memcpy(mmbuffer->ptr, data, size);
            mmbuffer->size = size;
        } else {
            free(mmbuffer);
            return NULL;
        }
    }
    return mmbuffer;
}

void destroy_mmbuffer(
  mmbuffer_t *mmbuffer
) {
    if (mmbuffer) {
        free(mmbuffer->ptr);
        free(mmbuffer);
    }
}

const char *get_mmbuffer_string(mmbuffer_t *mmbuffer) {
    if (!mmbuffer || !mmbuffer->ptr) return NULL;
    return mmbuffer->ptr;
}

// xpparam_t functions
xpparam_t *create_xpparam(
  unsigned long flags,
  xdl_regex_t **ignore_regex,
  size_t ignore_regex_count,
  char **anchors,
  size_t anchors_count
) {
    xpparam_t *xpparam = (xpparam_t *)malloc(sizeof(xpparam_t));
    if (xpparam) {
        xpparam->flags = flags;

        xpparam->ignore_regex = (xdl_regex_t **)malloc(ignore_regex_count * sizeof(xdl_regex_t *));
        if (xpparam->ignore_regex) {
            memcpy(xpparam->ignore_regex, ignore_regex, ignore_regex_count * sizeof(xdl_regex_t *));
        }
        xpparam->ignore_regex_nr = ignore_regex_count;

        xpparam->anchors = (char **)malloc(anchors_count * sizeof(char *));
        if (xpparam->anchors) {
            for (size_t i = 0; i < anchors_count; i++) {
                xpparam->anchors[i] = strdup(anchors[i]);
            }
        }
        xpparam->anchors_nr = anchors_count;
    }
    return xpparam;
}

void destroy_xpparam(
  xpparam_t *xpparam
) {
    if (xpparam) {
        // Free each regex in the ignore_regex array
        if (xpparam->ignore_regex) {
            for (size_t i = 0; i < xpparam->ignore_regex_nr; i++) {
                destroy_regex(xpparam->ignore_regex[i]);
            }
            free(xpparam->ignore_regex);
        }

        // Free each anchor string
        if (xpparam->anchors) {
            for (size_t i = 0; i < xpparam->anchors_nr; i++) {
                free(xpparam->anchors[i]);
            }
            free(xpparam->anchors);
        }

        free(xpparam);
    }
}

// xdemitcb_t functions
xdemitcb_t *create_xdemitcb(
  void *private_data,
  int (*output_hunk_callback)(
    void *,
    long old_begin,
    long old_count,
    long new_begin,
    long new_count,
    const char *function_name,
    long function_name_length
  ),
  int (*output_line_callback)(
    void *,
    mmbuffer_t *,
    int
  )
) {
    xdemitcb_t *xdemitcb = (xdemitcb_t *)malloc(sizeof(xdemitcb_t));
    if (xdemitcb) {
        xdemitcb->priv = private_data;
        xdemitcb->out_hunk = output_hunk_callback;
        xdemitcb->out_line = output_line_callback;
    }
    return xdemitcb;
}

void destroy_xdemitcb(
  xdemitcb_t *xdemitcb
) {
    if (xdemitcb) {
        free(xdemitcb);
    }
}

// xdemitconf_t functions
xdemitconf_t *create_xdemitconf(
  long context_length,
  long interhunk_context_length,
  unsigned long flags,
  find_func_t find_function,
  void *find_function_private,
  xdl_emit_hunk_consume_func_t hunk_function
) {
    xdemitconf_t *xdemitconf = (xdemitconf_t *)malloc(sizeof(xdemitconf_t));
    if (xdemitconf) {
        xdemitconf->ctxlen = context_length;
        xdemitconf->interhunkctxlen = interhunk_context_length;
        xdemitconf->flags = flags;
        xdemitconf->find_func = find_function;
        xdemitconf->find_func_priv = find_function_private;
        xdemitconf->hunk_func = hunk_function;
    }
    return xdemitconf;
}

void destroy_xdemitconf(
  xdemitconf_t *xdemitconf
) {
    if (xdemitconf) {
        free(xdemitconf);
    }
}

// xmparam_t functions
xmparam_t *create_xmparam(
  int marker_size,
  int merge_level,
  int favor_mode,
  int style,
  const char *ancestor_label,
  const char *file1_label,
  const char *file2_label
) {
    xmparam_t *xmparam = (xmparam_t *)malloc(sizeof(xmparam_t));
    if (xmparam) {
        xmparam->marker_size = marker_size;
        xmparam->level = merge_level;
        xmparam->favor = favor_mode;
        xmparam->style = style;
        xmparam->ancestor = strdup(ancestor_label);
        xmparam->file1 = strdup(file1_label);
        xmparam->file2 = strdup(file2_label);
    }
    return xmparam;
}

void destroy_xmparam(
  xmparam_t *xmparam
) {
    if (xmparam) {
        free((char *)xmparam->ancestor);
        free((char *)xmparam->file1);
        free((char *)xmparam->file2);
        free(xmparam);
    }
}

// Regex helper functions
xdl_regex_t *create_regex(
  const char *pattern,
  int flags
) {
    regex_t *regex = (regex_t *)malloc(sizeof(regex_t));
    if (regex) {
        int ret = regcomp(regex, pattern, flags);
        if (ret != 0) {
            free(regex);
            return NULL;
        }
    }
    return (xdl_regex_t *)regex;
}

void destroy_regex(
  xdl_regex_t *regex
) {
    if (regex) {
        regfree((regex_t *)regex);
        free(regex);
    }
}


// Node for the linked list of hunks
typedef struct xdiff_hunk_node {
    xdiff_hunk_t hunk;
    struct xdiff_hunk_node *next;
} xdiff_hunk_node_t;

// Updated xdiff_result_t with linked list and tail pointer for efficient appending
typedef struct xdiff_result_internal {
    xdiff_hunk_node_t *head; // Head of the linked list
    xdiff_hunk_node_t *tail; // Tail of the linked list for fast appending
    size_t hunk_count;       // Number of hunks in the list
} xdiff_result_internal_t;

// Appends a new hunk to the linked list
static void append_hunk(xdiff_result_internal_t *result, long old_begin, long old_count, long new_begin, long new_count) {
    if (!result) {
        fprintf(stderr, "Error: append_hunk received NULL result\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the new node
    xdiff_hunk_node_t *node = malloc(sizeof(xdiff_hunk_node_t));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed in append_hunk\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the hunk inside the node
    node->hunk.old_begin = old_begin;
    node->hunk.old_count = old_count;
    node->hunk.new_begin = new_begin;
    node->hunk.new_count = new_count;
    node->hunk.changes = NULL; // No changes yet
    node->hunk.change_count = 0;
    node->next = NULL;

    // Append the node to the linked list
    if (result->tail) {
        result->tail->next = node;
    } else {
        result->head = node;
    }

    result->tail = node;
    result->hunk_count++;
}

// Converts the linked list of hunks into an array
static xdiff_hunk_t *finalize_hunks(xdiff_result_internal_t *result) {
    if (!result || result->hunk_count == 0) {
        return NULL;
    }

    xdiff_hunk_t *hunks = malloc(result->hunk_count * sizeof(xdiff_hunk_t));
    if (!hunks) {
        fprintf(stderr, "Error: Memory allocation failed in finalize_hunks\n");
        exit(EXIT_FAILURE);
    }

    xdiff_hunk_node_t *current = result->head;
    for (size_t i = 0; i < result->hunk_count; i++) {
        if (!current) {
            fprintf(stderr, "Error: Linked list traversal failed in finalize_hunks\n");
            exit(EXIT_FAILURE);
        }
        memcpy(&hunks[i], &current->hunk, sizeof(xdiff_hunk_t));
        current = current->next;
    }

    return hunks;
}

// Frees the linked list of hunks
static void free_hunk_list(xdiff_result_internal_t *result) {
    if (!result) return;

    xdiff_hunk_node_t *current = result->head;
    while (current) {
        xdiff_hunk_node_t *next = current->next;

        for (size_t i = 0; i < current->hunk.change_count; i++) {
            free(current->hunk.changes[i].line);
        }
        free(current->hunk.changes);
        free(current);
        current = next;
    }
}

// Callback function for processing hunks
static int hunk_callback(void *priv, long old_begin, long old_count, long new_begin, long new_count, const char *func, long funclen) {
    // Cast private data to internal result structure
    xdiff_result_internal_t *result = (xdiff_result_internal_t *)priv;

    // Append the new hunk directly with parameters
    append_hunk(result, old_begin, old_count, new_begin, new_count);

    return 0; // Indicate success
}

// Callback function for processing lines within a hunk
static int line_callback(void *priv, mmbuffer_t *line, int type) {
    // Cast private data to internal result structure
    xdiff_result_internal_t *result = (xdiff_result_internal_t *)priv;

    // Ensure there's at least one hunk to associate this line with
    if (!result->tail) {
        fprintf(stderr, "Error: No hunk exists to associate the line\n");
        exit(EXIT_FAILURE);
    }

    xdiff_hunk_t *current_hunk = &result->tail->hunk; // Get the last hunk

    // Allocate memory for a new change
    current_hunk->changes = realloc(current_hunk->changes, (current_hunk->change_count + 1) * sizeof(xdiff_change_t));
    if (!current_hunk->changes) {
        fprintf(stderr, "Error: Memory allocation failed in line_callback\n");
        exit(EXIT_FAILURE);
    }

    // Add the new change
    xdiff_change_t *change = &current_hunk->changes[current_hunk->change_count++];
    change->type = (type == 0 ? ' ' : (type > 0 ? '+' : '-')); // Determine type of change
    change->line = strndup(line->ptr, line->size);             // Copy the line content

    return 0; // Indicate success
}

// Simplified wrapper for xdl_diff
xdiff_result_t *xdl_xdiff_simple(mmfile_t *mf1, mmfile_t *mf2, xpparam_t *xpp, xdemitconf_t *xecfg) {
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
        exit(EXIT_FAILURE);
    }

    result->hunks = finalize_hunks(&internal_result);
    result->hunk_count = internal_result.hunk_count;

    free_hunk_list(&internal_result);
    return result;
}

// Frees the memory allocated for the xdiff_result_t structure
void xdiff_result_destroy(xdiff_result_t *result) {
    if (!result) return;

    for (size_t i = 0; i < result->hunk_count; i++) {
        for (size_t j = 0; j < result->hunks[i].change_count; j++) {
            free(result->hunks[i].changes[j].line);
        }
        free(result->hunks[i].changes);
    }

    free(result->hunks);
    free(result);
}

// Getter for xdiff_change_t type
char xdiff_change_get_type(xdiff_change_t *change) {
    return change ? change->type : '\0'; // Return '\0' if change is NULL
}

// Getter for xdiff_change_t line
const char *xdiff_change_get_line(xdiff_change_t *change) {
    return change ? change->line : NULL; // Return NULL if change is NULL
}

// Getter for hunk's old begin line number
long xdiff_hunk_get_old_begin(xdiff_hunk_t *hunk) {
    return hunk ? hunk->old_begin : -1; // Return -1 if hunk is NULL
}

// Getter for hunk's old count
long xdiff_hunk_get_old_count(xdiff_hunk_t *hunk) {
    return hunk ? hunk->old_count : -1; // Return -1 if hunk is NULL
}

// Getter for hunk's new begin line number
long xdiff_hunk_get_new_begin(xdiff_hunk_t *hunk) {
    return hunk ? hunk->new_begin : -1;
}

// Getter for hunk's new count
long xdiff_hunk_get_new_count(xdiff_hunk_t *hunk) {
    return hunk ? hunk->new_count : -1;
}

// Getter for hunk's change count
size_t xdiff_hunk_get_change_count(xdiff_hunk_t *hunk) {
    return hunk ? hunk->change_count : 0;
}

// Getter for a specific change in a hunk
xdiff_change_t *xdiff_hunk_get_change_at(xdiff_hunk_t *hunk, size_t index) {
    if (hunk && index < hunk->change_count) {
        return &hunk->changes[index];
    }
    return NULL; // Return NULL if index is out of bounds
}

// Getter for result's hunk count
size_t xdiff_result_get_hunk_count(xdiff_result_t *result) {
    return result ? result->hunk_count : 0;
}

// Getter for a specific hunk in a result
xdiff_hunk_t *xdiff_result_get_hunk_at(xdiff_result_t *result, size_t index) {
    if (result && index < result->hunk_count) {
        return &result->hunks[index];
    }
    return NULL; // Return NULL if index is out of bounds
}
