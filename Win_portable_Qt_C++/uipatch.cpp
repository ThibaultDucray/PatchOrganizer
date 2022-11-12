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

#include "uipatch.h"

UIPatch::UIPatch(int index, int bank, int num, bool usIR, QString name) : QListWidgetItem(NULL, QListWidgetItem::UserType)
{
    set(index, bank, num, usIR, name);
}

void UIPatch::set(int index, int bank, int num, bool usIR, QString name) {
    this->index = index;
    this->bank = bank;
    this->num = num;
    this->usIR = usIR;
    this->name = name;
    this->pos = bank * 3 + num;
    setIcon(QIcon(usIR ? "img/cab_yes-128.svg" : "img/cab_no-128.svg"));
    setText(QString::number(bank) + "." + QString::number(num) + "\t" + name);
}

bool UIPatch::operator < (const UIPatch &other) const {
    return this->pos < other.pos;
}
