/* Needed to make strcasecmp() visible when compiling with a strict C
 * standard (e.g. -std=c11): strcasecmp is a POSIX extension, not part
 * of the C standard, so it must be requested explicitly by defining
 * this macro BEFORE including <string.h>. */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  /* strcasecmp() */
#include <ctype.h>

#include "config_parser.h"

/* Strips leading/trailing whitespace from a string, in place. */
static char *trim(char *s)
{
    if (s == NULL) return s;

    /* leading trim */
    while (isspace((unsigned char)*s)) s++;

    if (*s == '\0') return s;

    /* trailing trim */
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return s;
}

int config_load(const char *filename, Config *cfg)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "config_parser: cannot open file '%s'\n", filename);
        return -1;
    }

    /* default values, overwritten by whatever is found in the file */
    cfg->n = 0;
    cfg->chunk_size = 0;
    cfg->a = 0.0;
    cfg->x = 0.0;
    cfg->y = 0.0;
    strncpy(cfg->output_file, "output.h5", sizeof(cfg->output_file) - 1);
    cfg->output_file[sizeof(cfg->output_file) - 1] = '\0';

    char line[1024];
    int line_no = 0;
    int found_n = 0, found_chunk = 0, found_a = 0, found_x = 0, found_y = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        line_no++;

        char *l = trim(line);

        /* empty line or comment */
        if (l[0] == '\0' || l[0] == '#') continue;

        char *eq = strchr(l, '=');
        if (eq == NULL) {
            fprintf(stderr,
                    "config_parser: line %d ignored (missing '='): '%s'\n",
                    line_no, l);
            continue;
        }

        *eq = '\0';
        char *key = trim(l);
        char *value = trim(eq + 1);

        if (key[0] == '\0') {
            fprintf(stderr, "config_parser: line %d has an empty key\n", line_no);
            continue;
        }

        if (strcasecmp(key, "n") == 0) {
            cfg->n = atol(value);
            found_n = 1;
        } else if (strcasecmp(key, "chunk_size") == 0) {
            cfg->chunk_size = atol(value);
            found_chunk = 1;
        } else if (strcasecmp(key, "a") == 0) {
            cfg->a = atof(value);
            found_a = 1;
        } else if (strcasecmp(key, "x") == 0) {
            cfg->x = atof(value);
            found_x = 1;
        } else if (strcasecmp(key, "y") == 0) {
            cfg->y = atof(value);
            found_y = 1;
        } else if (strcasecmp(key, "output_file") == 0) {
            strncpy(cfg->output_file, value, sizeof(cfg->output_file) - 1);
            cfg->output_file[sizeof(cfg->output_file) - 1] = '\0';
        } else {
            fprintf(stderr,
                    "config_parser: unknown variable '%s' at line %d (ignored)\n",
                    key, line_no);
        }
    }

    fclose(fp);

    if (!found_n || !found_chunk || !found_a || !found_x || !found_y) {
        fprintf(stderr,
                "config_parser: incomplete configuration file "
                "(n, chunk_size, a, x, y are required)\n");
        return -1;
    }

    if (cfg->n <= 0 || cfg->chunk_size <= 0) {
        fprintf(stderr, "config_parser: n and chunk_size must be > 0\n");
        return -1;
    }

    return 0;
}

void config_print(const Config *cfg)
{
    printf("Configuration read:\n");
    printf("  n           = %ld\n", cfg->n);
    printf("  chunk_size  = %ld\n", cfg->chunk_size);
    printf("  a           = %g\n", cfg->a);
    printf("  x           = %g\n", cfg->x);
    printf("  y           = %g\n", cfg->y);
    printf("  output_file = %s\n", cfg->output_file);
}
