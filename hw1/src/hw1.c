#include <stdlib.h>

#include "debug.h"
#include "hw1.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the content of three frames of audio data and
 * two annotation fields have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variables "global_options", where they will be accessible
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

int goodCharRange(char *p);

int goodIntRange(int num);

int myatoi(char *p);

int goodHexRange(char *p);

unsigned long hexToDec(char *key);

void setBitForSpeedUp();

void setBitForSlowDown(int intFactor);

void setBitForHelp();

void setBitForPreserveAnnotation();

void setBitForCrypt(unsigned long key);

int validargs(int argc, char **argv)
{
    if (argc <= 1)
        return 0;
    else {
        char *pos_arg = *(argv + 1);
        char *option1;
        char *option2;
        char *factor;
        char *key;
        int intFactor = 0;
        unsigned long hexKey = 0;

        if((pos_arg = "-h")) {

            //SET BIT HERE
            return 1;

        }
        else if((pos_arg = "-u")) {

            //SET BIT HERE
            return 1;

        }
        else if((pos_arg = "-d")) {

             if((*(argv + 2))) {
                option1 = *(argv + 2);

                if((option1 = "-f")) {
                    if((*(argv + 3))) {

                        factor = *(argv + 3);

                        if(goodCharRange(factor)){
                            intFactor = myatoi(factor);

                            if(!(goodIntRange(intFactor)))
                                return 0;
                            else {
                                intFactor = intFactor - 1;

                                if((*(argv + 4))){
                                    option2 = *(argv + 4);

                                    if((option2 = "-p")){
                                        //DEAL WITH -p OPTION HERE
                                        setBitForSlowDown(intFactor);
                                        setBitForPreserveAnnotation();

                                        return 1;
                                    }
                                    else return 0;

                                }
                                else {
                                    setBitForSlowDown(intFactor);
                                    return 1;
                                }
                            }
                        }
                        else return 0;

                    }
                    else return 0;

                }
                else if((option1 = "-p")){
                    //DEAL WITH -p OPTION HERE
                    setBitForPreserveAnnotation();
                    return 1;

                }
                else return 0;

            }
            else {

                //SET BIT HERE
                setBitForSlowDown(intFactor);
                return 1;
            }

        }
        else if((pos_arg = "-c")) {

             if((*(argv + 2))) {
                option1 = *(argv + 2);

                if((option1 = "-k")) {
                    if((*(argv + 3))) {

                        key = *(argv + 3);

                        //PROCESS KEY VALUE HERE
                        if(goodHexRange(key)){

                            hexKey = hexToDec(key);

                            if((*(argv + 4))){
                                option2 = *(argv + 4);

                                if((option2 = "-p")){
                                        //DEAL WITH -p OPTION HERE
                                    setBitForCrypt(hexKey);
                                    setBitForPreserveAnnotation();

                                    return 1;
                                }
                                else return 0;

                            }
                            else {
                                setBitForSlowDown(intFactor);
                                return 1;
                            }
                        }
                        else return 0;

                    }
                    else return 0;

                }
                else return 0;

            }
            else {

                return 0;
            }
        }
    }
    return 0;
}

void setBitForHelp() {

    global_options = 1L<<63;
}

void setBitForSpeedUp() {

    global_options = 1L<<62;
}

void setBitForSlowDown(int intFactor) {

    global_options = 1L<<61;

    unsigned long factor = intFactor;

    factor = factor << 48;

    global_options = global_options | factor;

}

void setBitForCrypt(unsigned long key) {

    global_options = 1L<<60;
    global_options = global_options | key;
}

void  setBitForPreserveAnnotation() {

    unsigned long p_opt = 1L;
    p_opt = p_opt << 59;
    global_options = global_options | p_opt;

}

int goodCharRange(char *p) {

    while((*p != '\0')) {
        if(((*p < '0') || (*p > '9')))
            return 0;

        p++;
    }

    return 1;

}

int goodIntRange(int num) {

    if(((num < 1) || (num > 1024)))
        return 0;
    else return 1;

}

int goodHexRange(char *p) {
    int c = 0;

    while(((*p != '\0') && (c <= 8))) {
        if( ((*p >= '0') && (*p <= '9')) ||  ((*p >= 'A') && (*p <= 'Z')) || ((*p >= 'a') && (*p <= 'b')) ) {
            p++;
            c++;
        }
        else return 0;
    }

    return 1;
}

int myatoi(char *p) {

    int res = 0;

    while((*p != '\0')) {
        res = res*10 + (*p) - '0';

        p++;
    }

    return res;
}

unsigned long hexToDec(char *key) {

    unsigned long base = 1;
    unsigned long dec_val = 0;
    char *p = key;

    int c = 0;
    while((*p != '\0')){
        p++;
        c++;
    }
    p--;

    printf("value of c: %d\n", c);
    printf("value of p: %c\n", *p);

    while((c > 0)){

        if(((*p >= 'a') && (*p <= 'z'))){

            dec_val += ((*p) - 'W')*base;
            base = base*16;
            p--;
            c--;

        }
        else if(((*p >= 'A') && (*p <= 'Z'))){

            dec_val += ((*p) - '7')*base;
            base = base*16;
            p--;
            c--;

        }
        else {

            dec_val += ((*p) - '0')*base;
            base = base*16;
            p--;
            c--;
        }
    }

    return dec_val;
}


/**
 * @brief  Recodes a Sun audio (.au) format audio stream, reading the stream
 * from standard input and writing the recoded stream to standard output.
 * @details  This function reads a sequence of bytes from the standard
 * input and interprets it as digital audio according to the Sun audio
 * (.au) format.  A selected transformation (determined by the global variable
 * "global_options") is applied to the audio stream and the transformed stream
 * is written to the standard output, again according to Sun audio format.
 *
 * @param  argv  Command-line arguments, for constructing modified annotation.
 * @return 1 if the recoding completed successfully, 0 otherwise.
 */
int recode(char **argv) {
    return 0;
}

int read_header(AUDIO_HEADER *hp) {

    int c = 0;
    int shift = 24;
    int shiftCount = 0;
    unsigned int oneByte;

    unsigned int magic_number;
    unsigned int data_offset;
    unsigned int data_size;
    unsigned int encoding;
    unsigned int sample_rate;
    unsigned int channels;

    //READ THE WHOLE WORD
    do {
        if((shiftCount = 4) ){
            shiftCount = 0;
            shift = 24;
        }

        oneByte = getchar();
        if (feof(stdin) || ferror(stdin))
            return 0;
        else oneByte = oneByte << shift;

        if(c <= 3) {
            magic_number = magic_number | oneByte;
            shift = shift - 8;
            shiftCount++;
        }
        else if( ((c > 3) && (c <= 7)) ) {
            data_offset = data_offset | oneByte;
            shift = shift - 8;
            shiftCount++;
        }
        else if( ((c > 7) && (c <= 11)) ) {
            data_size = data_size | oneByte;
            shift = shift - 8;
            shiftCount++;

        }
        else if( ((c > 11) && (c <= 15)) ) {
            encoding = encoding | oneByte;
            shift = shift - 8;
            shiftCount++;

        }
        else if( ((c > 15) && (c <= 19)) ) {
            sample_rate = sample_rate | oneByte;
            shift = shift - 8;
            shiftCount++;

        }
        else {
            channels = channels | oneByte;
            shift = shift - 8;
            shiftCount++;

        }

        c++;

    } while(c < 24);

    hp -> magic_number = magic_number;
    hp -> data_offset = data_offset;
    hp -> data_size = data_size;
    hp -> encoding = encoding;
    hp -> sample_rate = sample_rate;
    hp -> channels = channels;

    //VALIDATE THE HEADER: RETURN 1 IF VALID, 0 OTHERWISE
    if((magic_number != 779316836))
        return 0;

    else if(((data_offset % 8) != 0))
        return 0;

    else if( !((encoding >= 2) && (encoding <= 5)) )
        return 0;

    else if( !((channels >= 1) && (channels <= 2)) )
        return 0;

    else return 1;

}

// int write_header(AUDIO_HEADER *hp) {

//     unsigned int magic_number = hp -> magic_number;
//     unsigned int data_offset = hp -> data_offset;
//     unsigned int data_size = hp -> data_size;
//     unsigned int encoding = hp -> encoding;
//     unsigned int sample_rate = hp -> sample_rate;
//     unsigned int channels = hp -> channels;

//     char *p;

//     int bit = 3;
//     int out;
//     int c = 0;

//     do{
//         if((bit = -1))
//             bit = 3;

//         if(c <= 3) {
//             p = (char)(&magic_number);
//             out = putchar(*(p + bit));
//             if( out==EOF || ferror(stdout) )
//                 return 0;

//             bit--;
//         }
//         else if( ((c > 3) && (c <= 7)) ) {
//             p = (char)&data_offset;
//             out = putchar(*(p + bit));
//             if( out==EOF || ferror(stdout) )
//                 return 0;

//             bit--;
//         }
//         else if( ((c > 7) && (c <= 11)) ) {
//             p = (char)&data_size;
//             out = putchar(*(p + bit));
//             if( out==EOF || ferror(stdout) )
//                 return 0;

//             bit--;

//         }
//         else if( ((c > 11) && (c <= 15)) ) {
//             p = (char)&encoding;
//             out = putchar(*(p + bit));
//             if( out==EOF || ferror(stdout) )
//                 return 0;

//             bit--;

//         }
//         else if( ((c > 15) && (c <= 19)) ) {
//             p = (char)&sample_rate;
//             out = putchar(*(p + bit));
//             if( out==EOF || ferror(stdout) )
//                 return 0;

//             bit--;

//         }
//         else {
//             p = (char)&channels;
//             out = putchar(*(p + bit));
//             if( out==EOF || ferror(stdout) )
//                 return 0;

//             bit--;

//         }

//         c++;

//     }while(c < 24);

//     return 1;

// return 0;
// }

int read_annotation(char *ap, unsigned int size) {

    unsigned int l = size;

    for(int i = 0; i < l; i++) {
        *ap = getchar();
        ap++;
        if (feof(stdin) || ferror(stdin))
            return 0;
        if(i == (l-1))
            if(*(ap) != '\0')
                return 0;
    }

    return 1;
}

int write_annotation(char *ap, unsigned int size) {

    unsigned int l = size;
    int out;

    for(int i = 0; i < l; i++) {
        out = putchar(*(ap + i));
        if ( out==EOF || ferror(stdout) )
            return 0;
    }

    return 1;
}

int read_frame(int *fp, int channels, int bytes_per_sample) {

    int totalBytes = channels * bytes_per_sample;
    int c = bytes_per_sample;

    // int x = 779316836;

    // int *num = &x;

    // char *p = (char*)num;

    int *buffer = fp;
    char *p = (char*)buffer;

    for(int i = 0; i < totalBytes; i++) {
        p* = getchar();
        if (feof(stdin) || ferror(stdin))
            return 0;
        if((*p) != 0x00)
            c--;

        if(c == 0){
            buffer++;
            p = (char*)buffer;
        }
    }

    return 1;
}

int write_frame(int *fp, int channels, int bytes_per_sample) {

    return 0;
}