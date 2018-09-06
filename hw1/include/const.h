/*
 * DO NOT MODIFY THE CONTENTS OF THIS FILE.
 * IT WILL BE REPLACED DURING GRADING
 */
#ifndef CONST_H
#define CONST_H

#include <stdio.h>

#define USAGE(program_name, retcode) do{ \
fprintf(stderr, "USAGE: %s %s\n", program_name, \
"[-h] -u|-d|-c [-f FACTOR] [-k KEY] [-p]\n" \
"    -h       Help: displays this help menu.\n" \
"    -u       Speed up: increase playback speed by deleting samples\n" \
"    -d       Slow down: decrease playback speed by inserting interpolated samples\n" \
"             Optional additional parameters with -u or -d:\n" \
"                -f            FACTOR is an integer factor for speed up or slow down\n" \
"                              It must be a decimal number in the range [1, 1024]\n" \
"                -p            Preserve input annotation without modification\n" \
"    -c       Crypt: encrypt or decrypt (requires -k)\n" \
"             Required additional parameter with -c:\n" \
"                -k            KEY is a secret key for encryption or decryption\n" \
"                              It must be a hexadecimal number with at most 8 digits\n" \
"             Optional additional parameter with -c:\n" \
"                -p            Preserve input annotation without modification\n"); \
exit(retcode); \
} while(0)

/* Options info, set by validargs. */
unsigned long global_options;

/*
 * Space for you to store data from two annotation fields: one for the input file
 * and one for the output file.
 */
#define ANNOTATION_MAX (1024)
char input_annotation[ANNOTATION_MAX];
char output_annotation[ANNOTATION_MAX];

/*
 * Space for you to store the sample data from three frames:
 * the current input frame, the current output frame, and a previous input
 * frame (the latter used when doing interpolation in "slow down mode").
 */
#define CHANNELS_MAX (2)
char previous_frame[CHANNELS_MAX * sizeof(int)];
char input_frame[CHANNELS_MAX * sizeof(int)];
char output_frame[CHANNELS_MAX * sizeof(int)];

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 1 if validation succeeds and 0 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv);

/**
 * @brief  Recodes a Sun audio (.au) format audio stream, reading the stream
 * from standard input and writing the recoded stream to standard output.
 * @details  This function reads a sequence of bytes from the standard
 * input and interprets it as digital audio according to the Sun audio
 * (.au) format.  A selected transformation (determined by the global variable
 * "global_options") is applied to the audio stream and the transformed stream
 * is written to the standard output, again according to Sun audio format.
 * Besides the transformation applied to the audio sample data, unless the -p
 * option was specified, the annotation data is transformed by prepending a
 * representation of the command-line arguments given to "audible".
 * If the transformed annotation would be longer than ANNOTATION_MAX,
 * it shall be considered an error and this function shall return 0 without
 * producing any output.  If the -p option was specified, then no transformation
 * is performed on the annotation, and the output annotation is identical to
 * the input annotation.
 *
 * @param  argv  Command-line arguments, for constructing modified annotation.
 * @return 1 if the recoding completed successfully, 0 otherwise.
 */
int recode(char **argv);

/**
 * @brief Read the header of a Sun audio file and check it for validity.
 * @details  This function reads 24 bytes of data from the standard input and
 * interprets it as the header of a Sun audio file.  The data is decoded into
 * six unsigned int values, assuming big-endian byte order.   The decoded values
 * are stored into the AUDIO_HEADER structure pointed at by hp.
 * The header is then checked for validity, which means:  no error occurred
 * while reading the header data, the magic number is valid, the data offset
 * is a multiple of 8, the value of encoding field is one of {2, 3, 4, 5},
 * and the value of the channels field is one of {1, 2}.
 *
 * @param hp  A pointer to the AUDIO_HEADER structure that is to receive
 * the data.
 * @return  1 if a valid header was read, otherwise 0.
 */
int read_header(AUDIO_HEADER *hp);

/**
 * @brief  Write the header of a Sun audio file to the standard output.
 * @details  This function takes the pointer to the AUDIO_HEADER structure passed
 * as an argument, encodes this header into 24 bytes of data according to the Sun
 * audio file format specifications, and writes this data to the standard output.
 *
 * @param  hp  A pointer to the AUDIO_HEADER structure that is to be output.
 * @return  1 if the function is successful at writing the data; otherwise 0.
 */
int write_header(AUDIO_HEADER *hp);

/**
 * @brief  Read annotation data for a Sun audio file from the standard input,
 * storing the contents in a specified buffer.
 * @details  This function takes a pointer 'ap' to a buffer capable of holding at
 * least 'size' characters, and it reads 'size' characters from the standard input,
 * storing the characters read in the specified buffer.  It is checked that the
 * data read is terminated by at least one null ('\0') byte.
 *
 * @param  ap  A pointer to the buffer that is to receive the annotation data.
 * @param  size  The number of bytes of data to be read.
 * @return  1 if 'size' bytes of valid annotation data were successfully read;
 * otherwise 0.
 */
int read_annotation(char *ap, unsigned int size);

/**
 * @brief  Write annotation data for a Sun audio file to the standard output.
 * @details  This function takes a pointer 'ap' to a buffer containing 'size'
 * characters, and it writes 'size' characters from that buffer to the standard
 * output.
 *
 * @param  ap  A pointer to the buffer containing the annotation data to be
 * written.
 * @param  size  The number of bytes of data to be written.
 * @return  1 if 'size' bytes of data were successfully written; otherwise 0.
 */
int write_annotation(char *ap, unsigned int size);

/**
 * @brief Read, from the standard input, a single frame of audio data having
 * a specified number of channels and bytes per sample.
 * @details  This function takes a pointer 'fp' to a buffer having sufficient
 * space to hold 'channels' values of type 'int', it reads
 * 'channels * bytes_per_sample' data bytes from the standard input,
 * interpreting each successive set of 'bytes_per_sample' data bytes as
 * the big-endian representation of a signed integer sample value, and it
 * stores the decoded sample values into the specified buffer.
 *
 * @param  fp  A pointer to the buffer that is to receive the decoded sample
 * values.
 * @param  channels  The number of channels.
 * @param  bytes_per_sample  The number of bytes per sample.
 * @return  1 if a complete frame was read without error; otherwise 0.
 */
int read_frame(int *fp, int channels, int bytes_per_sample);

/**
 * @brief  Write, to the standard output, a single frame of audio data having
 * a specified number of channels and bytes per sample.
 * @details  This function takes a pointer 'fp' to a buffer that contains
 * 'channels' values of type 'int', and it writes these data values to the
 * standard output using big-endian byte order, resulting in a total of
 * 'channels * bytes_per_sample' data bytes written.
 *
 * @param  fp  A pointer to the buffer that contains the sample values to
 * be written.
 * @param  channels  The number of channels.
 * @param  bytes_per_sample  The number of bytes per sample.
 * @return  1 if the complete frame was written without error; otherwise 0.
 */
int write_frame(int *fp, int channels, int bytes_per_sample);

#endif
