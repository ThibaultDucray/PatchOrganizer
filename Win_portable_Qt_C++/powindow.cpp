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

#include "powindow.h"
#include "./ui_powindow.h"
#include "renamedialog.h"
#include "uipatch.h"
#include <QMessageBox>

POWindow::POWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::POWindow)
{
    ui->setupUi(this);
    patchHandler = NULL;

    connect(ui->openButton, &QAbstractButton::clicked, this, &POWindow::openSlot);
    connect(ui->action_Open, &QAction::triggered, this, &POWindow::openSlot);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &POWindow::saveSlot);
    connect(ui->action_Save_as, &QAction::triggered, this, &POWindow::saveSlot);
    connect(ui->reloadButton, &QAbstractButton::clicked, this, &POWindow::reloadSlot);
    connect(ui->actionRe_load, &QAction::triggered, this, &POWindow::reloadSlot);
    connect(ui->exportButton, &QAbstractButton::clicked, this, &POWindow::exportSlot);
    connect(ui->action_Export_1_patch, &QAction::triggered, this, &POWindow::exportSlot);
    connect(ui->replaceButton, &QAbstractButton::clicked, this, &POWindow::replaceSlot);
    connect(ui->action_Replace_1_patch, &QAction::triggered, this, &POWindow::replaceSlot);
    connect(ui->action_Help, &QAction::triggered, this, &POWindow::help);
    connect(ui->action_About, &QAction::triggered, this, &POWindow::about);

    connect(ui->patchListWidget, &QListWidget::itemSelectionChanged, this, &POWindow::selectionChanged);
    connect(ui->patchListWidget, &QListWidget::itemDoubleClicked, this, &POWindow::doubleClicked);

    connect(ui->action_Quit, &QAction::triggered, qApp, &QCoreApplication::quit, Qt::QueuedConnection);

    vc = new VersionControler(VERSION);
    connect(vc->reply.get(), &QNetworkReply::finished, this, &POWindow::httpFinished);
}

POWindow::~POWindow()
{
    emptyList();
    delete vc;
    delete ui;
    if (patchHandler != NULL)
        delete patchHandler;
}

void POWindow::httpFinished() {
    vc->httpFinished();
}

void POWindow::emptyList() {
    while(ui->patchListWidget->count() > 0) {
        UIPatch * p = (UIPatch *) ui->patchListWidget->item(0);
        ui->patchListWidget->removeItemWidget(p);
        delete p;
    }
}

void POWindow::populateList() {
    emptyList();
    if (patchHandler->validFile) {
        for (int i = 0; i < patchHandler->nbPatches; i++) {
            PElem pelem = patchHandler->getElem(i);
            UIPatch *p = new UIPatch(i, pelem.bank, pelem.num, pelem.usIR, pelem.name);
            int j;
            for (j = 0; (j < ui->patchListWidget->count()) && (*(UIPatch *)(ui->patchListWidget->item(j)) < *p); j++) ;
            ui->patchListWidget->insertItem(j, p);
        }
    }
}

void POWindow::orderPatchList() {
    for (int i = 0; (i < ui->patchListWidget->count()) && (i < patchHandler->nbPatches); i++) {
        UIPatch *p = (UIPatch *) ui->patchListWidget->item(i);
        patchHandler->setElem(p->index, p->name, i);
    }
}

void POWindow::openSlot()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Preset File"), "", tr("Presets Files (*.prst)"));
    if (!fileName.isEmpty()) {
        patchHandler = new PatchHandler(fileName);
        if (patchHandler->validFile) {
            populateList();
            ui->fileNameLabel->setText("File: " + fileName);
            ui->reloadButton->setEnabled(patchHandler->nbPatches > 0);
            ui->actionRe_load->setEnabled(patchHandler->nbPatches > 0);
            ui->saveButton->setEnabled(patchHandler->nbPatches > 0);
            ui->action_Save_as->setEnabled(patchHandler->nbPatches > 0);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Invalid Ampero presets file.");
            msgBox.exec();
        }
    }
}

void POWindow::saveSlot()
{
    if (patchHandler != NULL) {
        QString fileName;
        fileName = QFileDialog::getSaveFileName(this, tr("Save Preset File"), patchHandler->actualFileName, tr("Presets Files (*.prst)"));
        if (!fileName.isEmpty()) {
            orderPatchList();
            int ret = patchHandler->writePatchList(fileName, ui->actionTailbit_inv_beta->isChecked());
            if (ret <= 0) {
                QMessageBox msgBox;
                msgBox.setText("Could not save presets file.");
                msgBox.exec();
            }
        }
    }
}

void POWindow::reloadSlot()
{
    if (patchHandler != NULL) {
        QString fileName = patchHandler->actualFileName;
        delete patchHandler;
        patchHandler = new PatchHandler(fileName);
        if (patchHandler->validFile) {
            populateList();
        } else {
            QMessageBox msgBox;
            msgBox.setText("Invalid Ampero presets file.");
            msgBox.exec();
        }
    }
}


void POWindow::selectionChanged()
{
    ui->exportButton->setEnabled(ui->patchListWidget->selectedItems().count() == 1);
    ui->action_Export_1_patch->setEnabled(ui->patchListWidget->selectedItems().count() == 1);
    ui->replaceButton->setEnabled(ui->patchListWidget->selectedItems().count() == 1);
    ui->action_Replace_1_patch->setEnabled(ui->patchListWidget->selectedItems().count() == 1);
    ui->selectionLabel->setText("Selected: " + QString::number(ui->patchListWidget->selectedItems().count()));
}


void POWindow::exportSlot()
{
    if (ui->patchListWidget->selectedItems().count() == 1) {
        QString fileName;
        UIPatch *p = (UIPatch *) ui->patchListWidget->selectedItems().first();
        fileName = QFileDialog::getSaveFileName(this, tr("Export Patch to File"), p->name, tr("Presets Files (*.prst)"));
        if (!fileName.isEmpty()) {
            // orderPatchList();
            int ret = patchHandler->exportOnePatch(fileName, p->index, ui->actionTailbit_inv_beta->isChecked());
            if (ret <= 0) {
                QMessageBox msgBox;
                msgBox.setText("Could not save preset file.");
                msgBox.exec();
            }
        }
    }
}


void POWindow::replaceSlot()
{
    if (ui->patchListWidget->selectedItems().count() == 1) {
        QString fileName;
        UIPatch *p = (UIPatch *) ui->patchListWidget->selectedItems().first();
        fileName = QFileDialog::getOpenFileName(this, tr("Open Preset File"), "", tr("Presets Files (*.prst)"));
        if (!fileName.isEmpty()) {
            PatchHandler *otherPH = new PatchHandler(fileName);
            if (otherPH->validFile && (otherPH->nbPatches == 1)) {
                bool ousir = otherPH->getElem(0).usIR;
                QString oname = otherPH->getElem(0).name;
                patchHandler->replacePatch(otherPH, 0, p->index);
                p->set(p->index, p->bank, p->num, ousir, oname);
            } else {
                QMessageBox msgBox;
                msgBox.setText("Please select a valid preset file containing only one patch.");
                msgBox.exec();
            }
            delete otherPH;
        }
    }
}


void POWindow::doubleClicked(QListWidgetItem *item)
{
    UIPatch *p = (UIPatch *) item;
    QString newName = p->name;
    RenameDialog *renameDialog = new RenameDialog(newName, this);
    renameDialog->exec();
    newName = renameDialog->newName;
    delete renameDialog;
    p->set(p->index, p->bank, p->num, p->usIR, newName);
}

void POWindow::help() {
    QDesktopServices::openUrl(QUrl("https://github.com/ThibaultDucray/PatchOrganizer/wiki"));
}

void POWindow::about() {
    QDesktopServices::openUrl(QUrl("https://github.com/ThibaultDucray/PatchOrganizer"));
}

