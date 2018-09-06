/*
 * DO NOT MODIFY THE CONTENTS OF THIS FILE.
 * IT WILL BE REPLACED DURING GRADING
 */
#ifndef AUDIO_H
#define AUDIO_H

/*
 * Sun audio file (.au) format:
 * All fields are stored in big-endian format, including the sample data.
 * The file begins with a header, consisting of six unsigned 32-bit words.
 *   The first field in the header is the "magic number", which must be
 *     equal to 0x2e736e64 (the four ASCII characters: ".snd").
 *   The second field in the header is the "data offset", which is the
 *     number of bytes from the beginning of the file that the audio sample
 *     data begins.  This value must be divisible by 8.  The minimum value
 *     is 24 (if there is no annotation field).  The minimum value if an
 *     annotation field exists is 32.
 *   The third field in the header is the "data size", which is the number
 *     of bytes of audio sample data.  The value 0xffffffff indicates that
 *     the size is unknown.
 *   The fourth field in the header specifies the encoding used for the
 *     audio samples.  We will only support the following values:
 *        2  (specifies 8-bit linear PCM encoding)
 *        3  (specifies 16-bit linear PCM encoding)
 *        4  (specifies 24-bit linear PCM encoding)
 *        5  (specifies 32-bit linear PCM encoding)
 *   The fifth field in the header specifies the "sample rate", which is the
 *     number of frames per second.
 *   The sixth field in the header specifies the number of audio channels.
 *     We will only support 1 (specifies mono) and 2 (specifies stereo).
 */

#define AUDIO_MAGIC (0x2e736e64)

#define PCM8_ENCODING (2)
#define PCM16_ENCODING (3)
#define PCM24_ENCODING (4)
#define PCM32_ENCODING (5)

typedef struct audio_header {
    unsigned int magic_number;
    unsigned int data_offset;
    unsigned int data_size;
    unsigned int encoding;
    unsigned int sample_rate;
    unsigned int channels;
} AUDIO_HEADER;

/*
 * Following the header is an optional "annotation field", which can be used
 * to store additional information (metadata) in the audio file.
 * The length of this field must be a multiple of eight bytes and it must be
 * terminated with at least one null ('\0') byte.  We will impose a maximum size
 * ANNOTATION_MAX on the annotation field.  When reading an audio file, if the
 * size of the annotation (i.e. input_header.data_offset - sizeof(AUDIO_HEADER))
 * exceeds this maximum, you should treat it as an error.  Similarly, if you would
 * be required to write an annotation area that exceeds the maximum size,
 * then this should also be treated as an error.
 */

/*
 * Audio data begins on an eight-byte boundary immediately following the
 * annotation field (or immediately following the header, if there is no
 * annotation field).  The audio data occurs as a sequence of *frames*,
 * where each frame contains data for one sample on each of the audio channels.
 * The size of a frame therefore depends both on the sample encoding and on
 * the number of channels.  For example, if the sample encoding is 16-bit PCM
 * (i.e. two bytes per sample) and the number of channels is two, then the number
 * of bytes in a frame will be 2 * 2 = 4.  If the sample encoding is 32-bit PCM
 * (i.e. four bytes per sample) and the number of channels is two, then the number
 * of bytes in a frame will be 2 * 4 = 8.  We will consider a partial frame at
 * the end of the file to be an error.  If the header specifies the data size,
 * then this size must be a multiple of the frame size, and there must be at least
 * that many bytes of audio data in the file; if not, we consider it as an error.
 * Extra data beyond the specified data size is not regarded as an error,
 * but any such data is to be ignored as if it were not present.
 *
 * Within a frame, the sample data for each channel occurs in sequence.
 * For example, in case of 16-bit PCM encoded stereo, the first two bytes of each
 * frame represents a single sample for channel 0 and the second two bytes
 * represents a single sample for channel 1.  Samples are signed values encoded
 * in two's-complement and are presented in big-endian (most significant byte first)
 * byte order.
 */

/*
 * File format:
 *
 * +------------------+--------------------------+-------+     +-------+
 * + Header (24 bytes)| Annotation (optional) \0 | Frame | ... | Frame |
 * +------------------+--------------------------+-------+     +-------+
 *                                               ^
 * <----------- data offset -------------------->(8-byte boundary)
 *
 * Frame format:
 *
 * +-----------------------------------+-----------------------------------+
 * | Sample 0 MSB | ... | Sample 0 LSB | Sample 1 MSB | ... | Sample 1 LSB |
 * +-----------------------------------+-----------------------------------+
 */

#endif
