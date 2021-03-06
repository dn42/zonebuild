/*
 * arg_opts.h
 *
 *  Created on: Dec 5, 2013
 *      Author: reboot
 */

#ifndef ARG_OPTS_H_
#define ARG_OPTS_H_


#define AR_VRP_OPT_NEGATE_MATCH         0x1
#define AR_VRP_OPT_TARGET_FD            0x2
#define AR_VRP_OPT_TARGET_LOOKUP        0x3

int
print_version_long(void *arg, int m, void *opt);
int
print_help(void *arg, int m, void *opt);

int
opt_g_stdout_lvl(void *arg, int m, void *opt);

#endif /* ARG_OPTS_H_ */
