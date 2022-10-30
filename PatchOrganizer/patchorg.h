// This file is part of PatchOrganizer.
//
// PatchOrganizer is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// PatchOrganizer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with PatchOrganizer.
// If not, see <https://www.gnu.org/licenses/>.
//
// (c) Copyright 2022 Thibault Ducray


#ifndef _PATCHORG_H_
#define _PATCHORG_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct filedesc {
    size_t size;
    //char *name;
    u_int8_t *content;
} Filedesc ;

// patch header is "MRAPxxxxxPxxNAME"
#define FILEHEADER_SIZE 824 // 32 + 99 * 8
#define FILEHEADER1_SIZE 32
#define PATCH_HEADER_SIZE 8
#define PATCH_NAME_SIZE 16 // maybe 16... 
#define PATCHHEADER
#define MAX_NBPATCHES 499 // today : 99. We have time...

typedef struct patch {
    u_int8_t mrap[4];// *header1; //[PATCHHEADER_SIZE]; // MRAP etc.
    u_int32_t size; // patch size
    u_int8_t sep; // '\01' separator ?
    u_int8_t pos; // [1]; // patch position (currently using 8 bits but might be 16... don't care for value from 0-99)
    u_int16_t sep2; // ??
    u_int8_t name[PATCH_NAME_SIZE]; // maybe 16... be careful there might not be a '\0' at the end
    u_int8_t data; // the rest of the data starting from here
} Patch ;

typedef struct userIR {
    u_int8_t risu[4];// string "RISU" = USIR = User IR
    u_int32_t size; // User IR desc size
    u_int8_t data; // the rest of the data starting from here
} UserIR;

typedef struct headerpatchdesc {
    u_int32_t offset;
    u_int32_t size;
} HeaderPatchDesc ;

typedef struct headerdesc {
    u_int8_t tsrp[4];
    u_int32_t size;
    u_int8_t pm0[5];
    u_int8_t sep;
    u_int16_t nbpatches;
    u_int8_t filler[16];
    //HeaderPatchDesc firstpatchdesc; // [NBPATCHES]; to be used as a pointer to the patches description. Can handle any number of patches
} HeaderDesc ;

typedef struct presetfile {
    HeaderDesc *header;
    HeaderPatchDesc *patchdesc;
    Patch *patches[MAX_NBPATCHES]; // Can handle great number of patches
    UserIR *userIRs[MAX_NBPATCHES]; // Can handle great number of patches. Some pointers in the array may be null
    u_int8_t *tail;
    size_t tailsize;
} PresetFile ;

// handler for external integration such as Swift
typedef struct patchlist {
    u_int16_t nbpatches;
    u_int8_t num[MAX_NBPATCHES]; // Can handle great number of patches
    u_int8_t userIR[MAX_NBPATCHES]; // Can handle great number of patches
    char name[MAX_NBPATCHES][PATCH_NAME_SIZE+1]; // Can handle great number of patches
    size_t fileSize;
    u_int8_t *fileContent;
    PresetFile presets;
} PatchList;

// functions declarations
size_t fileSize(const char *filename) ;
size_t readfile(u_int8_t* fileContent, const char* filename) ;
size_t calcPresetsFileSize(PresetFile *presets) ;
size_t writefile(u_int8_t* fileContent, const char* filename, size_t filesize) ;
int createPatch(struct patch* p, u_int8_t *raw, size_t size);
int createPatchList(PresetFile *presets, u_int8_t* fileContent, size_t filesize);
size_t createPresetfileContent(u_int8_t* fileContent, size_t filesize, PresetFile *presets, int invertTailBit);
void renumPatches(PresetFile* presets) ;
void permutPatches(PresetFile* presets, int src, int dest) ;
void orderPatches(PresetFile* presets, u_int8_t neworder[]) ; //[NBPATCHES]) ;

// utilities for swift integration
int readPresetsFromFile(const char *filename, PatchList *patchlist) ;
u_int8_t getPatchNumForIndex(PatchList *patchlist, int i) ;
u_int8_t getUserIRForIndex(PatchList *patchlist, int i) ;
void getPatchNameForIndex(char *name, PatchList *patchlist, int i) ;
void setPatchNumForIndex(PatchList *patchlist, int i, u_int8_t num) ;
void setPatchNameForIndex(PatchList *patchlist, int i, const char *name) ;
long int writePresetsToFile(const char *newfilename, PatchList *patchlist, int invertTailBit) ;


#endif
