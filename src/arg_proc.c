/*
 * arg_proc.c
 *
 *  Created on: Dec 5, 2013
 *      Author: reboot
 */

#include "arg_proc.h"
#include "arg_opts.h"

#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "l_error.h"

int execv_stdout_redir = -1;

mda ar_vref =
  { 0 };

int
default_determine_negated(void)
{
  if ( NULL != ar_find(&ar_vref, AR_VRP_OPT_NEGATE_MATCH))
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

int
g_cpg(void *arg, void *out, int m, size_t sz)
{
  char *buffer;
  if (m == 2)
    {
      buffer = (char *) arg;
    }
  else
    {
      buffer = ((char **) arg)[0];
    }
  if (!buffer)
    {
      return 1;
    }

  size_t a_l = strlen(buffer);

  if (!a_l)
    {
      return 2;
    }

  a_l > sz ? a_l = sz : sz;
  char *ptr = (char*) out;
  strncpy(ptr, buffer, a_l);
  ptr[a_l] = 0x0;

  return 0;
}

void *
g_pg(void *arg, int m)
{
  if (m == 2)
    {
      return (char *) arg;
    }
  return ((char **) arg)[0];
}

char *
g_pd(void *arg, int m, size_t l)
{
  char *buffer = (char*) g_pg(arg, m);
  char *ptr = NULL;
  size_t a_l = strlen(buffer);

  if (!a_l)
    {
      return NULL;
    }

  a_l > l ? a_l = l : l;

  if (a_l)
    {
      ptr = (char*) calloc(a_l + 1, 1);
      strncpy(ptr, buffer, a_l);
    }
  return ptr;
}

char **
build_argv(char *args, size_t max, int *c)
{
  char **ptr = (char **) calloc(max, sizeof(char **));

  size_t args_l = strlen(args);
  int i_0, l_p = 0, b_c = 0;
  char sp_1 = 0x20, sp_2 = 0x22, sp_3 = 0x60;

  *c = 0;

  for (i_0 = 0; i_0 <= args_l && b_c < max; i_0++)
    {
      if (i_0 == 0)
        {
          while (args[i_0] == sp_1)
            {
              i_0++;
            }
          if (args[i_0] == sp_2 || args[i_0] == sp_3)
            {
              while (args[i_0] == sp_2 || args[i_0] == sp_3)
                {
                  i_0++;
                }
              sp_1 = sp_2;
              l_p = i_0;
            }
        }

      if ((((args[i_0] == sp_1 || (args[i_0] == sp_2 || args[i_0] == sp_3))
          && args[i_0 - 1] != 0x5C && args[i_0] != 0x5C) || !args[i_0])
          && i_0 > l_p)
        {

          if (i_0 == args_l - 1)
            {
              if (!(args[i_0] == sp_1 || args[i_0] == sp_2 || args[i_0] == sp_3))
                {
                  i_0++;
                }

            }

          size_t ptr_b_l = i_0 - l_p;

          ptr[b_c] = (char*) calloc(ptr_b_l + 1, 1);
          strncpy((char*) ptr[b_c], &args[l_p], ptr_b_l);

          b_c++;
          *c += 1;

          int ii_l = 1;
          while (args[i_0] == sp_1 || args[i_0] == sp_2 || args[i_0] == sp_3)
            {
              if (sp_1 != sp_2 && sp_1 != sp_3)
                {
                  if (args[i_0] == sp_2 || args[i_0] == sp_3)
                    {
                      i_0++;
                      break;
                    }
                }
              i_0++;
            }
          l_p = i_0;
          if (sp_1 == sp_2 || sp_1 == sp_3)
            {
              sp_1 = 0x20;
              sp_2 = 0x22;
              sp_3 = 0x60;
              while (args[i_0] == sp_1 || args[i_0] == sp_2 || args[i_0] == sp_3)
                {
                  i_0++;
                }
              l_p = i_0;
            }
          else
            {
              if (args[i_0 - ii_l] == 0x22 || args[i_0 - ii_l] == 0x60)
                {
                  sp_1 = args[i_0 - ii_l];
                  sp_2 = args[i_0 - ii_l];
                  sp_3 = args[i_0 - ii_l];
                  while (args[i_0] == sp_1 || args[i_0] == sp_2
                      || args[i_0] == sp_3)
                    {
                      i_0++;
                    }
                  l_p = i_0;
                }

            }
        }

    }

  return ptr;
}

static int
process_opt_n(char *opt, void *arg, void *reference_array, int m, int *ret,
    void *data)
{
  int i = 0;
  __gg_opt ora = (__gg_opt) reference_array;

  while (ora->id)
    {
      size_t oo_l = strlen(ora->on);
      if (oo_l == strlen(opt) && !strncmp(ora->on, opt, oo_l))
        {
          if (ora->op)
            {
              *ret = i;
              return ora->op(arg, m, data);
            }
          else
            {
              return -4;
            }
        }

      ora++;
      i++;
    }
  return -2;
}

static void
ar_check_ttl_expired(pmda md)
{
  p_md_obj ptr = md_first(md);

  while (ptr)
    {
      __ar_vrp arp = (__ar_vrp) ptr->ptr;
      if ( arp->ttl == 0 )
        {
          p_md_obj s_ptr = ptr->next;
          md_unlink(md, ptr);
          ptr = s_ptr;
          continue;
        }
      ptr = ptr->next;
    }
}

__ar_vrp
ar_find(pmda md, uint32_t opt)
{
  p_md_obj ptr = md_first(md);

  while (ptr)
    {
      __ar_vrp arp = (__ar_vrp) ptr->ptr;
      if ( arp->opt == opt )
        {
          return arp;
        }
      ptr = ptr->next;
    }

  return NULL;
}

int
ar_remove(pmda md, uint32_t opt)
{
  p_md_obj ptr = md_first(md);

  while (ptr)
    {
      __ar_vrp arp = (__ar_vrp) ptr->ptr;
      if ( arp->opt == opt )
        {
          p_md_obj s_ptr = ptr->next;
          md_unlink(md, ptr);
          ptr = s_ptr;
          continue;

        }
      ptr = ptr->next;
    }

  return 1;
}

__ar_vrp
ar_add(pmda md, uint32_t opt, int ttl, void *arg)
{
  md_init(md, 8);

  __ar_vrp ptr = md_alloc(md, sizeof(_ar_vrp));

  ptr->opt = opt;
  ptr->ttl = ttl;
  ptr->arg = arg;

  return ptr;
}

void
ar_mod_ttl(pmda md, int by)
{
  p_md_obj ptr = md_first(md);

  while (ptr)
    {
      __ar_vrp arp = (__ar_vrp) ptr->ptr;
      if ( arp->ttl > 0)
        {
          arp->ttl += by;
        }
      ptr = ptr->next;
    }
}

#include <errno.h>

int
parse_args(int argc, char **argv, _gg_opt fref_t[], char ***la, uint32_t flags)
{
  g_setjmp(0, "parse_args", NULL, NULL);
  int vi, ret, c = 0;

  if (ar_vref.count)
    {
      md_free(&ar_vref);
    }

  md_init(&ar_vref, 8);

  int i;

  char *c_arg = NULL;
  char **c_argv = NULL;

  __gg_opt ora = (__gg_opt) fref_t;

  _g_vop vop =
    { 0 };

  errno = 0;

  for (i = 1, ret = 0, vi = -1; i < argc; i++, vi = -1)
    {
      c_arg = argv[i];
      c_argv = &argv[i];

      char *p_iseq = strchr(c_arg, 0x3D);

      if (p_iseq)
        {
          char bp_opt[64];
          size_t p_isl = p_iseq - c_arg;
          p_isl > 63 ? p_isl = 63 : 0;
          strncpy(bp_opt, c_arg, p_isl);
          bp_opt[p_isl] = 0x0;
          p_iseq++;

          ret = process_opt_n(bp_opt, p_iseq, fref_t, 2, &vi, (void*) &vop);
        }
      else
        {
          ret = process_opt_n(c_arg, (char*) &argv[i + 1], fref_t, 0, &vi,
              (void*) &vop);
        }

      ar_check_ttl_expired(&ar_vref);
      ar_mod_ttl(&ar_vref, -1);

      if (ret == -2)
        {
          if (flags & F_PARSE_ARG_IGNORE_NOT_FOUND)
            {
              continue;
            }
        }
      if (0 != ret)
        {
          if (!(flags & F_PARSE_ARG_SILENT))
            {
              if (ret == -2)
                {
                  fprintf(stderr, "ERROR: [%d] invalid option '%s'\n", ret,
                      c_arg);
                }
              else if (ret == -4)
                {
                  fprintf(stderr,
                      "ERROR: [%d] CRITICAL: improperly configured option ref table\n",
                      ret);
                  abort();
                }
              else if (ret > 0)
                {
                  fprintf(stderr, "ERROR: [%d] '%s': bad option argument\n",
                      ret, c_arg);
                }

            }
          if (!(flags & F_PARSE_ARG_IGNORE_ERRORS))
            {
              c = -2;
              break;
            }
        }
      else
        {
          c++;
        }

      if (NULL == p_iseq && vi > -1)
        {
          uint8_t ac = ora[vi].ac;
          if (0 != vop.ac_s)
            {
              ac += vop.ac_s;
              vop.ac_s = 0;
            }
          i += (int) ac;
        }
    }

  if (!(flags & F_PARSE_ARG_IGNORE_ERRORS) && !c)
    {
      return -1;
    }

  if ( NULL != la)
    {
      *la = (char**) c_argv;
    }

  return ret;
}

int
g_parse_opts(char *input, _p_opt_cb _proc, void *arg, int dl_o, int dl_v)
{
  mda m_dl_o =
    { 0 };
  md_init(&m_dl_o, 32);

  int ret = 0;

  int c = split_string(input, dl_o, &m_dl_o);

  if (c == 0)
    {
      ret = 1;
      goto end_1;
    }

  p_md_obj ptr_o = m_dl_o.objects;

  while (ptr_o)
    {
      char *opt = (char*) ptr_o->ptr;

      mda m_dl_v =
        { 0 };
      md_init(&m_dl_v, 8);

      if (split_string(opt, dl_v, &m_dl_v) == 0)
        {
          md_free(&m_dl_v);
          ret = 2;
          goto end_1;
        }

      if (m_dl_v.offset < 1)
        {
          ret = 3;
          goto end_1;
        }

      if (0 != _proc(&m_dl_v, arg))
        {
          ret = 4;
          goto end_1;
        }

      md_free(&m_dl_v);

      ptr_o = ptr_o->next;
    }

  end_1: ;

  md_free(&m_dl_o);

  return ret;
}
