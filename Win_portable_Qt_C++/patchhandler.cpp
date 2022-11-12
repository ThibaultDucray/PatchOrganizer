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

#include "patchhandler.h"

PatchHandler::PatchHandler(const QString &fileName)
{
    this->actualFileName = QString(fileName);
    this->newFileName = QString(fileName);
    filedesc.size = fileSize(actualFileName.toUtf8().constData());
    if (filedesc.size != 0)
        filedesc.content = new u_int8_t[filedesc.size];
    nbPatches = createPresetsFromFile(&presets, &filedesc, actualFileName.toUtf8().constData());
    validFile = nbPatches > 0;
}

PElem PatchHandler::getElem(int index) {
    PElem pelem;
    int num = getPatchNumForIndex(&presets, index);
    pelem.bank = num / 3 + 1;
    pelem.num = num - 3 * (pelem.bank - 1) + 1;
    pelem.usIR = getUserIRForIndex(&presets, index) != 0;
    char *cs = new char[PATCH_NAME_SIZE + 1];
    getPatchNameForIndex(cs, &presets, index);
    pelem.name = QString(cs);
    delete[] cs;
    return pelem;
}

void PatchHandler::setElem(int index, const QString &name, int newpos) {
    setPatchNumForIndex(&presets, index, newpos);
    setPatchNameForIndex(&presets, index, name.toUtf8().constData());
}

size_t PatchHandler::writePatchList(const QString &fileName, bool invertTailBit) {
    this->newFileName = fileName;
    size_t ret = writePresetsToFile(newFileName.toUtf8().constData(), &presets, invertTailBit ? 1 : 0);
    return ret;
}

size_t PatchHandler::exportOnePatch(QString fileName, int index, bool invertTailBit) {
    return writePresetsFileFromOnePatch(fileName.toUtf8().constData(), &presets, index, invertTailBit ? 1 : 0);
}

void PatchHandler::replacePatch(PatchHandler *source, int from, int to) {
    exchangePatchesInPreset(&presets, to, &(source->presets), from);
}

PatchHandler::~PatchHandler() {
    freePresetFile(&presets);
    if (filedesc.size != 0)
        delete[] filedesc.content;
}
