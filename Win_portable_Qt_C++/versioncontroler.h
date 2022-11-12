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

#ifndef VERSIONCONTROLER_H
#define VERSIONCONTROLER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QMessageBox>
#include <QDesktopServices>

class VersionControler
{
private:
    QNetworkAccessManager qnam;
    QString version;
public:
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    VersionControler(QString version);
    void httpFinished();
    int versionCompare(QString v1, QString v2);
};

#endif // VERSIONCONTROLER_H
