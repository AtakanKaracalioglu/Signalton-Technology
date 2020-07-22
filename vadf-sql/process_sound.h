/*
 * process_sound.h
 *
 *  Created on: Jun 25, 2020
 *      Author: kocak
 */

#ifndef PROCESS_SOUND_H_
#define PROCESS_SOUND_H_


#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fvad.h>
#include <sndfile.h>
#include <fstream>
#include <string>

bool process_sf(SNDFILE *infile, Fvad *vad,
    size_t framelen, FILE *listfile, long (&frames)[2], long (&segments)[2]);

//bool process_sf(SNDFILE *infile, Fvad *vad,
//    size_t framelen, SNDFILE *outfiles[2], FILE *listfile);

bool parse_int(int *dest, const char *s, int min, int max);




#endif /* PROCESS_SOUND_H_ */
