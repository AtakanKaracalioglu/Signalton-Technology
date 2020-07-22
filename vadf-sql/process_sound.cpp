/*
 * process_sound.cpp
 *
 *  Created on: Jun 25, 2020
 *      Author: kocak
 */
#include "process_sound.h"

bool process_sf(SNDFILE *infile, Fvad *vad,
    size_t framelen, FILE *listfile, long (&frames)[2], long (&segments)[2])
{
    bool success = false;
    double *buf0 = NULL;
    int16_t *buf1 = NULL;
    int vadres, prev = -1;
//    long frames[2] = {0, 0};
//    long segments[2] = {0, 0};

//    std::ofstream filestream;
//    filestream.open("predictions_m3_15db.txt");

    if (framelen > SIZE_MAX / sizeof (double)
            || !(buf0 = (double*) malloc(framelen * sizeof *buf0))
            || !(buf1 = (int16_t*) malloc(framelen * sizeof *buf1))) {
        fprintf(stderr, "failed to allocate buffers\n");
        goto end;
    }

    while (sf_read_double(infile, buf0, framelen) == (sf_count_t)framelen) {

        // Convert the read samples to int16
        for (size_t i = 0; i < framelen; i++)
            buf1[i] = buf0[i] * INT16_MAX;

        vadres = fvad_process(vad, buf1, framelen);
        if (vadres < 0) {
            fprintf(stderr, "VAD processing failed\n");
            goto end;
        }

//        if (listfile) {
//            fprintf(listfile, "%d\n", vadres);
//        }

        vadres = !!vadres; // make sure it is 0 or 1

//        if (outfiles[vadres]) {
//            sf_write_double(outfiles[!!vadres], buf0, framelen);
//        }
//        filestream<<std::to_string(vadres)<<std::endl;
        frames[vadres]++;
        if (prev != vadres) segments[vadres]++;
        prev = vadres;
    }

//    filestream.close();

//    printf("voice detected in %ld of %ld frames (%.2f%%)\n",
//        frames[1], frames[0] + frames[1],
//        frames[0] + frames[1] ?
//            100.0 * ((double)frames[1] / (frames[0] + frames[1])) : 0.0);
//    printf("%ld voice segments, average length %.2f frames\n",
//        segments[1], segments[1] ? (double)frames[1] / segments[1] : 0.0);
//    printf("%ld non-voice segments, average length %.2f frames\n",
//        segments[0], segments[0] ? (double)frames[0] / segments[0] : 0.0);

    success = true;

end:
    if (buf0) free(buf0);
    if (buf1) free(buf1);
    return success;
}




bool parse_int(int *dest, const char *s, int min, int max)
{
    char *endp;
    long val;

    errno = 0;
    val = strtol(s, &endp, 10);
    if (!errno && !*endp && val >= min && val <= max) {
        *dest = val;
        return true;
    } else {
        return false;
    }
}



