
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

void freePresetFile(PresetFile *presets) {
    int i;
    for (i = 0; i < presets->header->nbpatches; i++) {
        if (presets->patches[i] != NULL) free(presets->patches[i]);
    }
    if (presets->header != NULL) free(presets->header);
    memset(presets, 0, sizeof *presets);
}

// allocate the content of the PresetFile struct with given data in fileContent
// the PresetFile structure pointed be the given pointer will be erased
int createPresetsFromFile(PresetFile *presets, Filedesc *fd, const char *filename) {
    u_int8_t* fileContent;
    size_t filesize;
    size_t size, offset;
    void *alloc;
    int count;

    fileContent = fd->content;
    filesize = fd->size;
    size = readfile(fileContent, filename);
    if (size != filesize) {
        return 0;
    }
    
    // erase
    memset(presets, 0, sizeof *presets);

    // header
    presets->header = (HeaderDesc *) fileContent;
    size = sizeof *(presets->header) + presets->header->nbpatches * sizeof *(presets->patchdesc);
    alloc = malloc(size);
    presets->header = alloc;
    memcpy(presets->header, fileContent, sizeof *(presets->header));
    
    // patchdesc
    offset = sizeof *(presets->header);
    presets->patchdesc = alloc + offset;
    memcpy(presets->patchdesc, fileContent + offset, presets->header->nbpatches * sizeof *(presets->patchdesc));
    
    offset += presets->header->nbpatches * sizeof *(presets->patchdesc); // should be 824 for a 99 patches file

    for (count = 0; (count < presets->header->nbpatches) && (offset != 0) && (offset < filesize); count++) {
        offset = presets->patchdesc[count].offset;
        size = presets->patchdesc[count].size;
        alloc = malloc(size);
        presets->patches[count] = alloc;
        memcpy(presets->patches[count], fileContent + offset, size);
        if (presets->patches[count]->size + 8 < presets->patchdesc[count].size) { // presence of a User IR
            presets->userIRs[count] = (UserIR *) (alloc + presets->patches[count]->size + sizeof presets->patches[count]->mrap + sizeof presets->patches[count]->size);
        } else {
            presets->userIRs[count] = NULL;
        }
    }
    offset += size;
    if (filesize - offset != TAILSIZE) { // error in reading...
        count = 0;
        freePresetFile(presets);
    } else {
        memcpy(presets->tail, fileContent + offset, TAILSIZE);
    }
    return count;
}

// create a single patch presetfile struct
// all given structs must have been allocated
long int writePresetsFileFromOnePatch(const char *fileName, PresetFile *fromPresets, int index, int invertTailBit) {
    PresetFile presets;
    HeaderPatchDesc patchDesc;
    HeaderDesc header;
    long int err;
    
    memcpy(&header, fromPresets->header, sizeof header);
    presets.header = &header;
    patchDesc.size = fromPresets->patchdesc[index].size;
    header.size = sizeof header + sizeof patchDesc + patchDesc.size - sizeof header.tsrp - sizeof header.size;
    header.nbpatches = 1;
    patchDesc.offset = sizeof header + sizeof patchDesc;
    presets.patchdesc = &patchDesc;
    presets.patches[0] = malloc(patchDesc.size);
    memcpy(presets.patches[0], fromPresets->patches[index], patchDesc.size);
    presets.patches[0]->pos = 0x62; // = 98 as seen in 1-patch files (
    presets.tail[0] = fromPresets->tail[0];
    err = writePresetsToFile(fileName, &presets, invertTailBit);
    free(presets.patches[0]);
    return err;
}

size_t calcPresetsFileSize(PresetFile *presets) {
    size_t s;
    int i;

    // header
    s = sizeof(HeaderDesc);
    // patch desc
    s += presets->header->nbpatches * sizeof(HeaderPatchDesc);
    // content
    for (i = 0; i < presets->header->nbpatches; i++) {
        s += presets->patchdesc[i].size;
    }
    // tail
    s += TAILSIZE;
    return s;
}

// filesize to be calculated with calcPresetFileSize
// TODO - warning : not really sure of the file generation... (eg better manage offsets in headerpatchdesc and User IRs?)
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
            // header
            memcpy(fileContent, presets->header, sizeof *(presets->header));
            offset = sizeof *(presets->header);
            // patches descriptions
            for (i = 0; i < presets->header->nbpatches; i++) {
                memcpy(fileContent + offset, &(presets->patchdesc[i]), sizeof presets->patchdesc[i]);
                offset += sizeof presets->patchdesc[i];
            }
            
            // patches definitions (including user IR if any)
            for (i = 0; i < presets->header->nbpatches; i++) {
                p = presets->patches[i];
                memcpy(fileContent + offset, p, presets->patchdesc[i].size);
                offset += presets->patchdesc[i].size;
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

// it doesn't copy, it just affects new pointers
void exchangePatchesInPreset(PresetFile* destpresets, int dest, PresetFile *sourcepresets, int source) {
    u_int32_t oldPatchSize;
    u_int8_t oldPos;
    Patch *oldp;
    
    oldPatchSize = destpresets->patchdesc[dest].size;
    oldPos = destpresets->patches[dest]->pos;
    
    oldp = destpresets->patches[dest];
    destpresets->patchdesc[dest].size = sourcepresets->patchdesc[source].size;
    destpresets->patches[dest] = sourcepresets->patches[source];
    destpresets->patches[dest]->pos = oldPos;
    sourcepresets->patches[source] = oldp; // this allows the memory to be freed when freeing sourcepresets
    
    // recalc offset of next patch
    if (dest < destpresets->header->nbpatches - 1) {
        destpresets->patchdesc[dest + 1].offset = destpresets->patchdesc[dest + 1].offset - oldPatchSize + destpresets->patchdesc[dest].size;
    }
    // recalc file size
    destpresets->header->size = destpresets->header->size - oldPatchSize + destpresets->patchdesc[dest].size;
}

void orderPatches(PresetFile* presets, u_int8_t neworder[]) {
    int i;
    for (i = 0; i < presets->header->nbpatches; i++) {
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
u_int8_t getPatchNumForIndex(PresetFile *presets, int i) {
    return presets->patches[i]->pos;
}

// // utilities for external (eg. Swift) global encapsulation
u_int8_t getUserIRForIndex(PresetFile *presets, int i) {
    return presets->userIRs[i] != NULL ? 1 : 0;
    //return patchlist->userIR[i];
}

// utilities for external (eg. Swift) global encapsulation
void getPatchNameForIndex(char *name, PresetFile *presets, int i) {
    memset(name, 0, PATCH_NAME_SIZE + 1);
    memcpy(name, presets->patches[i]->name, PATCH_NAME_SIZE);
}

// utilities for external (eg. Swift) global encapsulation
void setPatchNumForIndex(PresetFile *presets, int i, u_int8_t num) {
    presets->patches[i]->pos = num;
    //patchlist->num[i] = num;
}

// utilities for external (eg. Swift) global encapsulation
void setPatchNameForIndex(PresetFile *presets, int i, const char *name) {
    char n[PATCH_NAME_SIZE + 1];
    
    memset(n, 0, PATCH_NAME_SIZE + 1);
    memcpy(n, name, strlen(name) > PATCH_NAME_SIZE ? PATCH_NAME_SIZE : strlen(name));
    memcpy(presets->patches[i]->name, n, PATCH_NAME_SIZE);
}

// utilities for external (eg. Swift) global encapsulation
long int writePresetsToFile(const char *newfilename, PresetFile *presets, int invertTailBit) {
    Filedesc fdnew;
    size_t newfilesize;

    //reorderAndRenamePatches(patchlist); // should have been done before, but we never know...

    // write to new file
    fdnew.size = calcPresetsFileSize(presets);
    fdnew.content = malloc(fdnew.size);
    if (fdnew.content == NULL) {
        return -5;
    }

    newfilesize = createPresetfileContent(fdnew.content, fdnew.size, presets, invertTailBit);
    if (newfilesize != fdnew.size) {
        free(fdnew.content);
        return -6;
    }

    newfilesize = writefile(fdnew.content, newfilename, fdnew.size);
    if (newfilesize != fdnew.size) {
        free(fdnew.content);
        return -7;
    }

    free(fdnew.content);
    return newfilesize;
}
