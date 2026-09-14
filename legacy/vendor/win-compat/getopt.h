/* Minimal getopt/getopt_long/getopt_long_only for MSVC/MinGW.
 * SPDX-License-Identifier: BSD-2-Clause */
#ifndef WIN_COMPAT_GETOPT_H
#define WIN_COMPAT_GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

#define no_argument        0
#define required_argument  1
#define optional_argument  2

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
extern int optreset;

int getopt(int argc, char *const argv[], const char *optstring);
int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex);
int getopt_long_only(int argc, char *const argv[], const char *optstring,
                     const struct option *longopts, int *longindex);

#ifdef __cplusplus
}
#endif

#endif /* WIN_COMPAT_GETOPT_H */
