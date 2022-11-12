#include "aboutdialog.h"
#include "./ui_aboutdialog.h"
#include <QDesktopServices>

AboutDialog::AboutDialog(QString version, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->versionLabel->setText(version);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}


void AboutDialog::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/ThibaultDucray/PatchOrganizer"));
}

