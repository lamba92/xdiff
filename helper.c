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

// Helper function to handle null checks and throw an error
static void handle_null_check(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

// Getter functions for xdiff_change_t
char xdiff_change_get_type(xdiff_change_t *change) {
    if (!change) handle_null_check("xdiff_change_t is NULL");
    return change->type;
}

const char *xdiff_change_get_line(xdiff_change_t *change) {
    if (!change) handle_null_check("xdiff_change_t is NULL");
    return change->line;
}

// Getter functions for xdiff_hunk_t
long xdiff_hunk_get_old_begin(xdiff_hunk_t *hunk) {
    if (!hunk) handle_null_check("xdiff_hunk_t is NULL");
    return hunk->old_begin;
}

long xdiff_hunk_get_old_count(xdiff_hunk_t *hunk) {
    if (!hunk) handle_null_check("xdiff_hunk_t is NULL");
    return hunk->old_count;
}

long xdiff_hunk_get_new_begin(xdiff_hunk_t *hunk) {
    if (!hunk) handle_null_check("xdiff_hunk_t is NULL");
    return hunk->new_begin;
}

long xdiff_hunk_get_new_count(xdiff_hunk_t *hunk) {
    if (!hunk) handle_null_check("xdiff_hunk_t is NULL");
    return hunk->new_count;
}

size_t xdiff_hunk_get_change_count(xdiff_hunk_t *hunk) {
    if (!hunk) handle_null_check("xdiff_hunk_t is NULL");
    return hunk->change_count;
}

xdiff_change_t *xdiff_hunk_get_change_at(xdiff_hunk_t *hunk, size_t index) {
    if (!hunk) handle_null_check("xdiff_hunk_t is NULL");
    if (index >= hunk->change_count) handle_null_check("Index out of bounds in xdiff_hunk_get_change_at");
    return &hunk->changes[index];
}

// Getter functions for xdiff_result_t
size_t xdiff_result_get_hunk_count(xdiff_result_t *result) {
    if (!result) handle_null_check("xdiff_result_t is NULL");
    return result->hunk_count;
}

xdiff_hunk_t *xdiff_result_get_hunk_at(xdiff_result_t *result, size_t index) {
    if (!result) handle_null_check("xdiff_result_t is NULL");
    if (index >= result->hunk_count) handle_null_check("Index out of bounds in xdiff_result_get_hunk_at");
    return &result->hunks[index];
}

// Callback function to handle hunk information during the diff operation.
static int hunk_callback(void *priv, long old_begin, long old_count, long new_begin, long new_count, const char *func, long funclen) {
    xdiff_result_t *result = (xdiff_result_t *)priv; // Cast private data to xdiff_result_t

    // Allocate memory for a new hunk in the result
    result->hunks = realloc(result->hunks, (result->hunk_count + 1) * sizeof(xdiff_hunk_t));
    if (!result->hunks) return -1; // Allocation error

    // Initialize the new hunk
    xdiff_hunk_t *hunk = &result->hunks[result->hunk_count++];
    hunk->old_begin = old_begin;
    hunk->old_count = old_count;
    hunk->new_begin = new_begin;
    hunk->new_count = new_count;
    hunk->changes = NULL;
    hunk->change_count = 0;

    return 0;
}

// Callback function to handle line changes during the diff operation.
static int line_callback(void *priv, mmbuffer_t *line, int type) {
    xdiff_result_t *result = (xdiff_result_t *)priv; // Cast private data to xdiff_result_t

    // Ensure there is at least one hunk to associate the line with
    if (result->hunk_count == 0) return -1;

    xdiff_hunk_t *hunk = &result->hunks[result->hunk_count - 1]; // Get the last hunk

    // Allocate memory for a new change in the hunk
    hunk->changes = realloc(hunk->changes, (hunk->change_count + 1) * sizeof(xdiff_change_t));
    if (!hunk->changes) return -1; // Allocation error

    // Initialize the new change
    xdiff_change_t *change = &hunk->changes[hunk->change_count++];
    change->type = (type == 0 ? ' ' : (type > 0 ? '+' : '-')); // Determine the type of change
    change->line = strndup(line->ptr, line->size); // Copy the line content

    return 0;
}

// Simplified wrapper for xdl_diff that returns a structured result instead of using callbacks.
xdiff_result_t *xdl_xdiff_simple(mmfile_t *mf1, mmfile_t *mf2, xpparam_t *xpp, xdemitconf_t *xecfg) {
    // Allocate memory for the result structure
    xdiff_result_t *result = malloc(sizeof(xdiff_result_t));
    if (!result) return NULL; // Allocation error

    result->hunks = NULL;
    result->hunk_count = 0;

    // Define the callbacks for hunks and lines
    xdemitcb_t ecb = {
            .priv = result,
            .out_hunk = hunk_callback,
            .out_line = line_callback
    };

    // Perform the diff operation
    if (xdl_diff(mf1, mf2, xpp, xecfg, &ecb) < 0) {
        // Cleanup in case of error
        for (size_t i = 0; i < result->hunk_count; i++) {
            for (size_t j = 0; j < result->hunks[i].change_count; j++) {
                free(result->hunks[i].changes[j].line); // Use destroy for each line
            }
            free(result->hunks[i].changes); // Use destroy for the changes array
        }
        free(result->hunks); // Use destroy for the hunks array
        free(result); // Use destroy for the result structure
        return NULL;
    }

    return result;
}

// Frees the memory allocated for the xdiff_result_t structure.
void xdiff_result_destroy(xdiff_result_t *result) {
    if (!result) return; // Nothing to free

    // Free each hunk and its associated changes
    for (size_t i = 0; i < result->hunk_count; i++) {
        for (size_t j = 0; j < result->hunks[i].change_count; j++) {
            free(result->hunks[i].changes[j].line); // Use destroy for the line content
        }
        free(result->hunks[i].changes); // Use destroy for the changes array
    }

    free(result->hunks); // Use destroy for the hunks array
    free(result); // Use destroy for the result structure
}
