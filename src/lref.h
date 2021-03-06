/*
 * lref.h
 *
 *  Created on: Dec 4, 2013
 *      Author: reboot
 */

#ifndef LREF_H_
#define LREF_H_


#include "l_error.h"

#include "m_comp.h"


#define MAX_SHARG_SZ    4096

void *
as_ref_to_val_lk(char *match, void *c, __d_drt_h mppd, char *defdc);
char *
g_get_stf(char *match);

typedef void*
rtv_af(void *arg, char *match, char *output, size_t max_size, __d_drt_h mppd);

rtv_af ref_to_val_af, ref_to_val_af_math;

int
rtv_q(void *query, char *output, size_t max_size);

char*
l_mppd_shell_ex(char *input, char *output, size_t max_size, void **l_nr, char l,
    char r, uint32_t flags);

#define F_MPPD_SHX_TZERO        ((uint32_t)1 << 1)

#define LMS_EX_L          0x28
#define LMS_EX_R          0x29

#define PROC_SH_EX(match) \
{ \
char m_b[MAX_SHARG_SZ]; \
  void *l_next_ref = NULL; \
  if (NULL == (match = l_mppd_shell_ex(match, m_b, MAX_SHARG_SZ, &l_next_ref, LMS_EX_L, LMS_EX_R, 0x0))) \
    { \
      return NULL; \
    } \
    if (NULL != mppd && l_next_ref != NULL) { \
        ((__d_drt_h) mppd)->varg_l = (char*)l_next_ref; \
    }\
};

char *
g_extract_vfield(char *input, char *output, size_t max_size, size_t offset);
void *
l_mppd_create_copy(__d_drt_h mppd);

size_t
l_mppd_gvlen(char *input);

typedef struct ___std_lref_header {

} _std_lrh, *__std_lrh;

#endif /* LREF_H_ */
