/* Minimal getopt/getopt_long/getopt_long_only for MSVC/MinGW.
 * Adapted from BSD-licensed sources (NetBSD libc, musl).
 * SPDX-License-Identifier: BSD-2-Clause */
#include "getopt.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;
int optreset = 0;

static const char *nextchar = NULL;

static void report(const char *fmt, ...) {
    va_list ap;
    if (!opterr) return;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
}

int getopt(int argc, char *const argv[], const char *optstring) {
    if (optreset || nextchar == NULL || *nextchar == '\0') {
        optreset = 0;
        if (optind >= argc) return -1;
        const char *arg = argv[optind];
        if (arg[0] != '-' || arg[1] == '\0') return -1;
        if (arg[1] == '-' && arg[2] == '\0') { optind++; return -1; }
        nextchar = arg + 1;
        optind++;
    }

    int c = (unsigned char)*nextchar++;
    const char *spec = strchr(optstring, c);
    optopt = c;

    if (c == ':' || c == '?' || spec == NULL) {
        report("%s: unknown option -- %c", argv[0], c);
        return '?';
    }

    if (spec[1] == ':') {
        if (*nextchar != '\0') {
            optarg = (char *)nextchar;
            nextchar = NULL;
        } else if (spec[2] == ':') {
            optarg = NULL;
        } else if (optind < argc) {
            optarg = argv[optind++];
        } else {
            report("%s: option requires an argument -- %c", argv[0], c);
            return optstring[0] == ':' ? ':' : '?';
        }
    } else {
        optarg = NULL;
    }
    return c;
}

static int parse_long(int argc, char *const argv[], const char *optstring,
                      const struct option *longopts, int *longindex,
                      int long_only) {
    if (nextchar != NULL && *nextchar != '\0') return getopt(argc, argv, optstring);
    if (optreset || optind >= argc) { optreset = 0; return -1; }

    const char *arg = argv[optind];
    if (arg[0] != '-') return -1;

    int is_long = 0;
    const char *name = NULL;
    if (arg[1] == '-') {
        if (arg[2] == '\0') { optind++; return -1; }
        is_long = 1;
        name = arg + 2;
    } else if (long_only) {
        is_long = 1;
        name = arg + 1;
    } else {
        return getopt(argc, argv, optstring);
    }

    size_t namelen = 0;
    while (name[namelen] != '\0' && name[namelen] != '=') namelen++;

    const struct option *match = NULL;
    int match_count = 0;
    for (const struct option *o = longopts; o != NULL && o->name != NULL; o++) {
        if (strncmp(name, o->name, namelen) == 0) {
            if (strlen(o->name) == namelen) { match = o; match_count = 1; break; }
            match = o;
            match_count++;
        }
    }

    if (long_only && match_count == 0) {
        return getopt(argc, argv, optstring);
    }
    if (match_count == 0) {
        report("%s: unknown option -- %.*s", argv[0], (int)namelen, name);
        optind++;
        return '?';
    }
    if (match_count > 1) {
        report("%s: ambiguous option -- %.*s", argv[0], (int)namelen, name);
        optind++;
        return '?';
    }

    const char *arg_start = name + namelen;
    if (*arg_start == '=') arg_start++;
    optind++;

    int has_arg = match->has_arg;
    if (has_arg == no_argument) {
        if (*arg_start != '\0') {
            report("%s: option doesn't take an argument -- %s", argv[0], match->name);
            return '?';
        }
        optarg = NULL;
    } else if (has_arg == required_argument) {
        if (*arg_start != '\0') {
            optarg = (char *)arg_start;
        } else if (optind < argc) {
            optarg = argv[optind++];
        } else {
            report("%s: option requires an argument -- %s", argv[0], match->name);
            return optstring[0] == ':' ? ':' : '?';
        }
    } else {
        optarg = (*arg_start != '\0') ? (char *)arg_start : NULL;
    }

    if (longindex != NULL) longindex[0] = (int)(match - longopts);
    if (match->flag != NULL) { *match->flag = match->val; return 0; }
    return match->val;
}

int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex) {
    return parse_long(argc, argv, optstring, longopts, longindex, 0);
}

int getopt_long_only(int argc, char *const argv[], const char *optstring,
                     const struct option *longopts, int *longindex) {
    return parse_long(argc, argv, optstring, longopts, longindex, 1);
}
