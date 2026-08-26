#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

/*
 * config_parser.h
 * ----------------
 * Small parser for text configuration files with the syntax:
 *
 *     Variable = Value
 *
 * One variable per line. Empty lines and lines starting with '#'
 * are ignored (so comments can be added to the file).
 *
 * This is a "homemade" parser meant to stay self-contained (no
 * external library to download). As an alternative, as suggested in
 * the assignment, one could use an existing library such as
 * https://github.com/welljsjs/Config-Parser-C : the Config struct
 * below is designed so that swapping it in later only requires
 * touching config_parser.c.
 */

typedef struct {
    long   n;            /* length of vectors d, x, y                */
    long   chunk_size;   /* size of each chunk                        */
    double a;            /* scalar a                                  */
    double x;            /* constant value of every element of x      */
    double y;            /* constant value of every element of y      */
    char   output_file[512]; /* path of the output HDF5 file          */
} Config;

/* Reads the configuration file 'filename' and fills 'cfg'.
 * Returns 0 on success, -1 on error (file not found, missing value,
 * etc.). On error, prints a diagnostic message to stderr. */
int config_load(const char *filename, Config *cfg);

/* Prints the content of a Config to stdout (useful for debug/log). */
void config_print(const Config *cfg);

#endif /* CONFIG_PARSER_H */
