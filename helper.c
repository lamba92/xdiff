#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

const char *get_mmbuffer_string(
  mmbuffer_t *mmbuffer
) {
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
        if (xpparam->ignore_regex) {
            for (size_t i = 0; i < xpparam->ignore_regex_nr; i++) {
                destroy_regex(xpparam->ignore_regex[i]);
            }
            free(xpparam->ignore_regex);
        }

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
