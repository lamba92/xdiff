#ifndef HELPER_H
#define HELPER_H

#include <stddef.h>
#include "xdiff.h"

// mmfile_t functions
mmfile_t *create_mmfile(
  const char *data,
  long size
);
void free_mmfile(
  mmfile_t *mmfile
);

// mmbuffer_t functions
mmbuffer_t *create_mmbuffer(
  const char *data,
  long size
);
void free_mmbuffer(
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
void free_xpparam(
  xpparam_t *xpparam
);
void destroy_recursively_xpparam(
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
void free_xdemitcb(
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
void free_xdemitconf(
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
void free_xmparam(
  xmparam_t *xmparam
);

// Regex helper functions
xdl_regex_t *create_regex(
  const char *pattern,
  int flags
);
void free_regex(
  xdl_regex_t *regex
);

#endif // HELPER_H
