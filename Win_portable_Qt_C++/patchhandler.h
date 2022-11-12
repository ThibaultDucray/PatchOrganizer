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

#ifndef PATCHHANDLER_H
#define PATCHHANDLER_H

#include <QString>

extern "C" {
    #include "patchorg.h"
}

typedef struct pelem {
    int bank;
    int num;
    bool usIR;
    QString name;
} PElem;

class PatchHandler
{
private:
    PresetFile presets;
    Filedesc filedesc;

public:
    QString actualFileName;
    QString newFileName;
    bool validFile;
    int nbPatches;
    PatchHandler(const QString &fileName);
    PElem getElem(int index);
    void setElem(int index, const QString &name, int newPos);
    size_t writePatchList(const QString &fileName, bool invertTailBit) ;
    size_t exportOnePatch(QString fileName, int index, bool invertTailBit) ;
    void replacePatch(PatchHandler *source, int from, int to) ;
    ~PatchHandler();
};

#endif // PATCHHANDLER_H
