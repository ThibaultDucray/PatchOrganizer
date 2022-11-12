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

#include "versioncontroler.h"

VersionControler::VersionControler(QString version)
{
    this->version = version;
    QUrl url = QUrl("https://api.github.com/repos/ThibaultDucray/PatchOrganizer/releases/latest");
    reply.reset(qnam.get(QNetworkRequest(url)));
    //connect(reply.get(), &QNetworkReply::finished, this, &VersionControler::httpFinished);
    qDebug() << "request sent";
}

void VersionControler::httpFinished()
{
    qDebug() << "getting value";
    // read data
    QString ReplyText = reply->readAll();
    // ask doc to parse it
    QJsonDocument doc = QJsonDocument::fromJson(ReplyText.toUtf8());
    // we know first element in file is object, to try to ask for such
    QJsonObject obj = doc.object();
    // ask object for value
    QJsonValue value = obj.value(QString("tag_name"));
    qDebug() << "get value is" << value.toString();;
    reply->reset(); // make sure to clean up

    if (versionCompare(version, value.toString()) < 0) {
        QMessageBox msgBox;
        msgBox.setText("New version detected: " + value.toString());
        msgBox.setInformativeText("You are running version " + version + ".\nDo you want to visit the download web page?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Yes:
              QDesktopServices::openUrl(QUrl("https://github.com/ThibaultDucray/PatchOrganizer/releases"));
              break;
          case QMessageBox::No:
              break;
          default:
              // should never be reached
              break;
        }
    }
}

int VersionControler::versionCompare(QString v1, QString v2)
{
    // vnum stores each numeric
    // part of version
    int vnum1 = 0, vnum2 = 0;

    // loop until both string are
    // processed
    for (int i = 0, j = 0; (i < v1.length()
                            || j < v2.length());) {
        // storing numeric part of
        // version 1 in vnum1
        while (i < v1.length() && v1[i] != '.') {
            vnum1 = vnum1 * 10 + (v1[i].unicode() - '0');
            i++;
        }

        // storing numeric part of
        // version 2 in vnum2
        while (j < v2.length() && v2[j] != '.') {
            vnum2 = vnum2 * 10 + (v2[j].unicode() - '0');
            j++;
        }

        if (vnum1 > vnum2)
            return 1;
        if (vnum2 > vnum1)
            return -1;

        // if equal, reset variables and
        // go for next numeric part
        vnum1 = vnum2 = 0;
        i++;
        j++;
    }
    return 0;
}
