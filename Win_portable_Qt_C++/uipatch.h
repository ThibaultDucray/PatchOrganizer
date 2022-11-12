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

#ifndef UIPATCH_H
#define UIPATCH_H

#include <QListWidgetItem>

class UIPatch : public QListWidgetItem
{
public:
    int index;
    int pos;
    int bank;
    int num;
    bool usIR;
    QString name;
    UIPatch(int index, int bank, int num, bool usIR, QString name);
    bool operator < (const UIPatch &other) const;
    QString getText();
    void set(int index, int bank, int num, bool usIR, QString name);
};

#endif // UIPATCH_H
