#ifndef HELPER_H
#define HELPER_H

#include <stddef.h>
#include "xdiff.h"

// mmfile_t functions
mmfile_t *create_mmfile(
  const char *data,
  long size
);
void destroy_mmfile(
  mmfile_t *mmfile
);

// mmbuffer_t functions
mmbuffer_t *create_mmbuffer(
  const char *data,
  long size
);
void destroy_mmbuffer(
  mmbuffer_t *mmbuffer
);
const char *get_mmbuffer_string(
mmbuffer_t *mmbuffer
);

// xpparam_t functions
xpparam_t *create_xpparam(
  unsigned long flags,
  xdl_regex_t **ignore_regex,
  size_t ignore_regex_count,
  char **anchors,
  size_t anchors_count
);
void destroy_xpparam(
  xpparam_t *xpparam
);

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
);
void destroy_xdemitcb(
  xdemitcb_t *xdemitcb
);

// xdemitconf_t functions
xdemitconf_t *create_xdemitconf(
  long context_length,
  long interhunk_context_length,
  unsigned long flags,
  find_func_t find_function,
  void *find_function_private,
  xdl_emit_hunk_consume_func_t hunk_function
);
void destroy_xdemitconf(
  xdemitconf_t *xdemitconf
);

// xmparam_t functions
xmparam_t *create_xmparam(
  int marker_size,
  int merge_level,
  int favor_mode,
  int style,
  const char *ancestor_label,
  const char *file1_label,
  const char *file2_label
);
void destroy_xmparam(
  xmparam_t *xmparam
);

// Regex helper functions
xdl_regex_t *create_regex(
  const char *pattern,
  int flags
);
void destroy_regex(
  xdl_regex_t *regex
);

// Struct representing a single line change in a hunk
typedef struct xdiff_change {
    char type; // '+' for addition, '-' for deletion, ' ' for unchanged
    char *line; // Pointer to the content of the line
} xdiff_change_t;

// Struct representing a single hunk containing metadata and an array of changes
typedef struct xdiff_hunk {
    long old_begin; // Start line number in the old file
    long old_count; // Number of lines in the old file hunk
    long new_begin; // Start line number in the new file
    long new_count; // Number of lines in the new file hunk
    xdiff_change_t *changes; // Array of changes in this hunk
    size_t change_count; // Number of changes in the array
} xdiff_hunk_t;

// Struct representing the entire diff result containing all hunks
typedef struct xdiff_result {
    xdiff_hunk_t *hunks; // Array of hunks
    size_t hunk_count; // Number of hunks in the array
} xdiff_result_t;

// Getter functions for xdiff_change_t
char xdiff_change_get_type(xdiff_change_t *change);
const char *xdiff_change_get_line(xdiff_change_t *change);

// Getter functions for xdiff_hunk_t
long xdiff_hunk_get_old_begin(xdiff_hunk_t *hunk);
long xdiff_hunk_get_old_count(xdiff_hunk_t *hunk);
long xdiff_hunk_get_new_begin(xdiff_hunk_t *hunk);
long xdiff_hunk_get_new_count(xdiff_hunk_t *hunk);
xdiff_change_t *xdiff_hunk_get_changes(xdiff_hunk_t *hunk);
size_t xdiff_hunk_get_change_count(xdiff_hunk_t *hunk);

// Getter functions for xdiff_result_t
xdiff_hunk_t *xdiff_result_get_hunks(xdiff_result_t *result);
size_t xdiff_result_get_hunk_count(xdiff_result_t *result);

// Simplified wrapper for xdl_diff that returns a structured result
xdiff_result_t *xdl_xdiff_simple(mmfile_t *mf1, mmfile_t *mf2, xpparam_t *xpp, xdemitconf_t *xecfg);

// Frees the memory allocated for the xdiff_result_t structure
void xdiff_result_destroy(xdiff_result_t *result);

#endif // HELPER_H
