
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

#include "patchorg.h"

size_t readfile(u_int8_t* fileContent, const char* filename) {
    FILE* f;
    size_t fs, frs;

    f = fopen(filename, "rb");
    if (f == NULL) {
        return 0;
    }
    fseek(f, 0L, SEEK_END);
    fs = ftell(f);
    rewind(f);
    frs = fread(fileContent, 1, fs, f);
    if (frs != fs) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return frs;
}

size_t presetsFileSize(PresetFile *presets) {
    return presets->header->size + sizeof presets->header->tsrp + sizeof presets->header->size;
}

size_t writefile(u_int8_t* fileContent, const char* filename, size_t filesize) {
    FILE *f;
    size_t i;
    f = fopen(filename, "wb");
    if (f == NULL)
        return 0;
    i = fwrite(fileContent, 1, filesize, f);
    fclose(f);
    return i;
}

int createPatchList(PresetFile *presets, u_int8_t* fileContent, size_t filesize) {
    size_t offset, j;
    int count;

    // header
    j = 0;
    presets->header = (HeaderDesc *) fileContent;
    offset = 1;
    for (count = 0; (count < NBPATCHES) && (offset != 0) && (offset < filesize); count++) {
        offset = presets->header->patchdesc[count].offset;
        j = presets->header->patchdesc[count].size;
        presets->patches[count] = (Patch *) (fileContent + offset);
        if (presets->patches[count]->size + 8 < presets->header->patchdesc[count].size) { // presence of a User IR
            presets->userIRs[count] = (UserIR *) (fileContent + offset + presets->patches[count]->size + 8);
        } else {
            presets->userIRs[count] = NULL;
        }
    }
    offset += j;
    presets->tail = fileContent + offset;
    presets->tailsize = filesize - offset; // should be 3
    return count;
}

size_t calcPresetsFileSize(PresetFile *presets) {
    size_t s;
    int i;

    // header
    s = sizeof(HeaderDesc);
    // content
    for (i = 0; i < NBPATCHES; i++) {
        s += presets->header->patchdesc[i].size;
    }
    // tail
    s += presets->tailsize;
    return s;
}

// filesize to be calculated with calcPresetFileSize
// TODO - warning : not really sure of the file generation... (eg manage User IR)
size_t createPresetfileContent(u_int8_t* fileContent, size_t filesize, PresetFile *presets, int invertTailBit) {
    int i;
    Patch *p;
    size_t offset;
    size_t offset2;
    u_int8_t sum;
    u_int8_t tb;

    // copy data into dest buffer
    offset = 0;
    if (filesize > 0) {
        if (fileContent != NULL) {
            memcpy(fileContent, presets->header, sizeof *(presets->header));
            offset = sizeof *(presets->header);
            for (i = 0; i < NBPATCHES; i++) {
                p = presets->patches[i];
                memcpy(fileContent + offset, p, presets->header->patchdesc[i].size);
                offset += presets->header->patchdesc[i].size;
            }
            // copy tailbit - don't know what it is
            if (invertTailBit) {
                tb = *(presets->tail) ? 0 : 1;
            } else {
                tb = *(presets->tail);
            }
            *(fileContent + offset++) = tb; // don't know what it is
            // These 2 bytes (eg 06 0A) represent the result of a 8bit checksum from offset 0 to offset (filesize-2), eg 6A.
            // byte 1 (eg. 06) is the high quartet (= 0xF & checksum >> 4)
            // byte 2 (eg. 0A) is the low quartet (= 0xF & checksum )
            sum = 0;
            for (offset2 = 0; offset2 < filesize - 2; offset2++) {
                sum += *(fileContent + offset2);
            }
            *(fileContent + offset++) = 0xF & (sum >> 4);
            *(fileContent + offset++) = 0xF & sum;
        } else
            offset = 0;
    }
    return offset;
}

void orderPatches(PresetFile* presets, u_int8_t neworder[NBPATCHES]) {
    int i;
    for (i = 0; i < NBPATCHES; i++) {
        presets->patches[i]->pos = neworder[i];
    }
}

size_t fileSize(const char *filename) {
    FILE *f;
    size_t s;

    f = fopen(filename, "rb");
    if (f == NULL) {
        return 0;
    }

    fseek(f, 0L, SEEK_END);
    s = ftell(f);
    fclose(f);
    return s;
}

// utilities for external (eg. Swift) global encapsulation
int readPresetsFromFile(const char *filename, PatchList *patchlist) {
    Filedesc fd;
    PresetFile presets;
    int err, i;
    size_t filesize;

    fd.size = fileSize(filename);
    if (fd.size == 0) {
        return -1;
    }
    fd.content = malloc(fd.size);
    if (fd.content == NULL) {
        return 0;
    }
    filesize = readfile(fd.content, filename);
    if (filesize != fd.size) {
        return -1;
    }

    err = createPatchList(&presets, fd.content, fd.size);
    if (err != NBPATCHES) {
        free(fd.content);
        return 0;
    }

    for (i = 0; i < NBPATCHES; i++) {
        patchlist->num[i] = presets.patches[i]->pos;
        patchlist->userIR[i] = presets.userIRs[i] != NULL ? 1 : 0;
        patchlist->name[i][PATCH_NAME_SIZE] = '\0';
        memcpy(patchlist->name[i], presets.patches[i]->name, PATCH_NAME_SIZE);
    }

    free(fd.content);

    return err;
}

// utilities for external (eg. Swift) global encapsulation
u_int8_t getPatchNumForIndex(PatchList *patchlist, int i) {
    return (patchlist->num[i]);
}

// // utilities for external (eg. Swift) global encapsulation
u_int8_t getUserIRForIndex(PatchList *patchlist, int i) {
    return patchlist->userIR[i];
}

// utilities for external (eg. Swift) global encapsulation
void getPatchNameForIndex(char *name, PatchList *patchlist, int i) {
    strcpy(name, patchlist->name[i]);
}

// utilities for external (eg. Swift) global encapsulation
void setPatchNumForIndex(PatchList *patchlist, int i, u_int8_t num) {
    patchlist->num[i] = num;
}

// utilities for external (eg. Swift) global encapsulation
void setPatchNameForIndex(PatchList *patchlist, int i, const char *name) {
    char n[PATCH_NAME_SIZE + 1];
    
    memset(n, 0, PATCH_NAME_SIZE + 1);
    memcpy(n, name, strlen(name) > PATCH_NAME_SIZE ? PATCH_NAME_SIZE : strlen(name));
    memcpy(patchlist->name[i], n, PATCH_NAME_SIZE);
}

// utilities for external (eg. Swift) global encapsulation
long int writePresetsToFile(const char *newfilename, const char *oldfilename, PatchList *patchlist, int invertTailBit) {
    Filedesc fdold;
    Filedesc fdnew;
    PresetFile presets;
    int err, i;
    size_t oldfilesize, newfilesize;

    // read from actual file
    fdold.size = fileSize(oldfilename);
    if (fdold.size == 0) {
        return -1;
    }
    fdold.content = malloc(fdold.size);
    if (fdold.content == NULL) {
        return -2;
    }
    oldfilesize = readfile(fdold.content, oldfilename);
    if (oldfilesize != fdold.size) {
        free(fdold.content);
        return -3;
    }

    err = createPatchList(&presets, fdold.content, fdold.size);
    if (err != NBPATCHES) {
        free(fdold.content);
        return -4;
    }

    for (i = 0; i < NBPATCHES; i++) {
        presets.patches[i]->pos = patchlist->num[i];
        // ewrite names - may be hazardous
        memcpy(presets.patches[i]->name, patchlist->name[i], PATCH_NAME_SIZE);
    }

    // write to new file
    fdnew.size = calcPresetsFileSize(&presets);
    fdnew.content = malloc(fdnew.size);
    if (fdnew.content == NULL) {
        free(fdold.content);
        return -5;
    }

    newfilesize = createPresetfileContent(fdnew.content, fdnew.size, &presets, invertTailBit);
    if (newfilesize != fdnew.size) {
        free(fdold.content);
        free(fdnew.content);
        return -6;
    }

    newfilesize = writefile(fdnew.content, newfilename, fdnew.size);
    if (newfilesize != fdnew.size) {
        free(fdold.content);
        free(fdnew.content);
        return -7;
    }

    free(fdold.content);
    free(fdnew.content);
    return newfilesize;
}
