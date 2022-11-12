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

#ifndef POWINDOW_H
#define POWINDOW_H

#include "patchhandler.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QModelIndex>
#include <QListWidgetItem>
#include "versioncontroler.h"

#define VERSION "1.0.4"

QT_BEGIN_NAMESPACE
namespace Ui { class POWindow; }
QT_END_NAMESPACE

class POWindow : public QMainWindow
{
    Q_OBJECT

public:
    POWindow(QWidget *parent = nullptr);
    void emptyList();
    void populateList();
    void orderPatchList();
    ~POWindow();

private slots:
    void openSlot();
    void saveSlot();
    void reloadSlot();
    void exportSlot();
    void replaceSlot();

    void selectionChanged();
    void doubleClicked(QListWidgetItem *item);

    void help();
    void about();

    void httpFinished();
private:
    Ui::POWindow *ui;
    PatchHandler *patchHandler;
    VersionControler *vc;
};
#endif // POWINDOW_H
