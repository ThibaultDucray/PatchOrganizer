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

#include "renamedialog.h"
#include "./ui_renamedialog.h"

RenameDialog::RenameDialog(const QString &actualName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameDialog)
{
    ui->setupUi(this);
    this->actualName = actualName;
    this->newName = actualName;
    ui->nameLabel->setText("Rename patch " + actualName);
    ui->nameEdit->setText(actualName);
}

RenameDialog::~RenameDialog()
{
    delete ui;
}

void RenameDialog::on_buttonBox_accepted()
{
    newName = ui->nameEdit->text();
}

