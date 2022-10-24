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
#define PATCH_NAME_SIZE 32 // maybe 32... 
#define PATCHHEADER
#define NBPATCHES 99

typedef struct patch {
    u_int8_t mrap[4];// *header1; //[PATCHHEADER_SIZE]; // MRAP etc.
    u_int32_t size; // patch size
    u_int8_t sep; // '\01' separator ?
    u_int8_t pos; // [1]; // patch position (currently using 8 bits but might be 16... don't care for value from 0-99)
    u_int16_t sep2; // ??
    u_int8_t name[PATCH_NAME_SIZE]; // maybe 32... be careful there might not be a 
    u_int8_t data; // the rest of the data starting from here
} Patch ;

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
    HeaderPatchDesc patchdesc [NBPATCHES];
} HeaderDesc ;

typedef struct presetfile {
    HeaderDesc *header;
    Patch *patches[NBPATCHES];
    u_int8_t *tail;
    size_t tailsize;
} PresetFile ;

// only numbers and names
typedef struct patchlist {
    u_int8_t num[NBPATCHES];
    char name[NBPATCHES][PATCH_NAME_SIZE+1];
} PatchList;

// functions declarations
size_t fileSize(char *filename) ;
size_t readfile(u_int8_t* fileContent, char* filename) ;
size_t calcPresetsFileSize(PresetFile *presets) ;
size_t writefile(u_int8_t* fileContent, char* filename, size_t filesize) ;
int createPatch(struct patch* p, u_int8_t *raw, size_t size);
int createPatchList(PresetFile *presets, u_int8_t* fileContent, size_t filesize);
size_t createPresetfileContent(u_int8_t* fileContent, size_t filesize, PresetFile *presets);
void renumPatches(PresetFile* presets) ;
void permutPatches(PresetFile* presets, int src, int dest) ;
void orderPatches(PresetFile* presets, u_int8_t neworder[NBPATCHES]) ;

// utilities for swift integration
int readPresetsFromFile(char *filename, PatchList *patchlist) ;
u_int8_t getPatchNumForIndex(PatchList *patchlist, int i) ;
void getPatchNameForIndex(char *name, PatchList *patchlist, int i) ;
void setPatchNumForIndex(PatchList *patchlist, int i, u_int8_t num) ;
size_t writePresetsToFile(char *newfilename, char *oldfilename, PatchList *patchlist) ;


#endif