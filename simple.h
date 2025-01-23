#ifndef SIMPLE_H
#define SIMPLE_H

#include <stddef.h>
#include "xdiff.h"

// Struct representing a single hunk containing metadata and an array of lines
typedef struct xdiff_hunk {
    long old_begin;     // Start line number in the old file
    long old_count;     // Number of lines in the old file hunk
    long new_begin;     // Start line number in the new file
    long new_count;     // Number of lines in the new file hunk
    char **lines;       // Array of null-terminated strings
    size_t line_count;  // Number of lines in the array
} xdiff_hunk_t;

// Struct representing the entire diff result containing all hunks
typedef struct xdiff_result {
    xdiff_hunk_t *hunks; // Array of hunks
    size_t hunk_count;   // Number of hunks in the array
} xdiff_result_t;

// Getter functions for xdiff_hunk_t
size_t xdiff_hunk_get_line_count(
  xdiff_hunk_t *hunk
);
const char *xdiff_hunk_get_line_at(
  xdiff_hunk_t *hunk,
  size_t index
);

// Getter functions for xdiff_result_t
size_t xdiff_result_get_hunk_count(
  xdiff_result_t *result
);
xdiff_hunk_t *xdiff_result_get_hunk_at(
  xdiff_result_t *result,
  size_t index
);

// Simplified wrapper for xdl_diff that returns a structured result
xdiff_result_t *xdl_xdiff_simple(
  mmfile_t *mf1,
  mmfile_t *mf2,
  xpparam_t *xpp,
  xdemitconf_t *xecfg
);

// Frees the memory allocated for the xdiff_result_t structure
void xdiff_result_destroy(
    xdiff_result_t *result
);

#endif // SIMPLE_H