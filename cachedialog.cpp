#include "cachedialog.h"
#include <QtConcurrent/QtConcurrent>
#include <qtconcurrentrun.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QClipboard>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

QClipboard *cacheclipboard = nullptr;

int cval1;

QString cachecontent;

bool cacheDialog::xmlcheck() {
   return xmlcheckBox->isChecked();
}

bool cacheDialog::cacheclip() {
   return cacheclipboard;
}

int cacheDialog::cbuffermode() {
   return buffermode->currentIndex();
}

QString cacheDialog::cbufferfactor() {
   return bufferfactor->text();
}

QString cacheDialog::cbuffersize() {
   return buffersize->text();
}

void cacheDialog::setcbuffersize(const QString &cbuffersize)
{
    buffersize->setText(cbuffersize);
}

void cacheDialog::setcbuffermode(const int &cbuffermode)
{
    buffermode->setCurrentIndex(cbuffermode);
}

void cacheDialog::setcbufferfactor(const QString &cbufferfactor)
{
    bufferfactor->setText(cbufferfactor);
}

void cacheDialog::setxmlcheck(const bool &xmlcheck)
{
    xmlcheckBox->setChecked(xmlcheck);
}

int cacheDialog::returncval1() {
   return cval1;
}

cacheDialog::cacheDialog(QWidget *parent) :
    QDialog(parent)
{
    setFixedSize(466, 240);
    setWindowTitle("Cache settings");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(8);

    titleLabel = new QLabel("Kodi cache settings");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(8);

    QLabel *modeLabel = new QLabel("Mode");
    grid->addWidget(modeLabel, 0, 0);

    buffermode = new QComboBox();
    buffermode->addItems({"", "0", "1", "2", "3"});
    buffermode->setToolTip("Kodi cache mode");
    grid->addWidget(buffermode, 0, 1);

    xmlcheckBox = new QCheckBox("Old cache xml");
    xmlcheckBox->setToolTip("Check this box for v16 and older cache xml");
    grid->addWidget(xmlcheckBox, 0, 2);

    QLabel *sizeLabel = new QLabel("Size");
    grid->addWidget(sizeLabel, 1, 0);

    buffersize = new QLineEdit();
    buffersize->setInputMask("999999999");
    buffersize->setToolTip("Kodi cache size in bytes");
    grid->addWidget(buffersize, 1, 1);

    QLabel *factorLabel = new QLabel("Read factor");
    grid->addWidget(factorLabel, 2, 0);

    bufferfactor = new QLineEdit();
    bufferfactor->setInputMask("9999");
    bufferfactor->setToolTip("Kodi Cache read factor");
    grid->addWidget(bufferfactor, 2, 1);

    QHBoxLayout *presetLayout = new QHBoxLayout();
    cpreset1 = new QRadioButton("PS1");
    cpreset1->setToolTip("Preset 1");
    cpreset2 = new QRadioButton("PS2");
    cpreset2->setToolTip("Preset 2");
    cpreset3 = new QRadioButton("PS3");
    cpreset3->setToolTip("Preset 3");

    presetLayout->addWidget(cpreset1);
    presetLayout->addWidget(cpreset2);
    presetLayout->addWidget(cpreset3);
    grid->addLayout(presetLayout, 3, 0, 1, 3);

    mainLayout->addLayout(grid);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    writecxml = new QPushButton("Write XML");
    writecxml->setToolTip("Create advancedsettings.xml file");

    copyxml = new QPushButton("Copy");
    copyxml->setToolTip("Copy cache XML to clipboard");

    resetcxml = new QPushButton("Reset");
    resetcxml->setToolTip("Reset cache settings to default");

    cancelButton = new QPushButton("Cancel");

    buttonLayout->addWidget(writecxml);
    buttonLayout->addWidget(copyxml);
    buttonLayout->addWidget(resetcxml);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(cpreset1, &QRadioButton::clicked, this, &cacheDialog::on_cpreset1_clicked);
    connect(cpreset2, &QRadioButton::clicked, this, &cacheDialog::on_cpreset2_clicked);
    connect(cpreset3, &QRadioButton::clicked, this, &cacheDialog::on_cpreset3_clicked);
    connect(writecxml, &QPushButton::clicked, this, &cacheDialog::on_writecxml_clicked);
    connect(copyxml, &QPushButton::clicked, this, &cacheDialog::on_copyxml_clicked);
    connect(resetcxml, &QPushButton::clicked, this, &cacheDialog::on_resetcxml_clicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

cacheDialog::~cacheDialog()
{
}

void cacheDialog::on_cpreset1_clicked()
{
    setcbuffermode(1);
    setcbuffersize("0");
    setcbufferfactor("10");
}

void cacheDialog::on_cpreset2_clicked()
{
    setcbuffermode(2);
    setcbuffersize("104857600");
    setcbufferfactor("1");
}

void cacheDialog::on_cpreset3_clicked()
{
    setcbuffermode(2);
    setcbuffersize("52428800");
    setcbufferfactor("1");
}

void cacheDialog::on_writecxml_clicked()
{
   cval1 = 2;
   QDialog::accept();
}

void cacheDialog::on_resetcxml_clicked()
{
    buffersize->setText("20971520");
    buffermode->setCurrentIndex(2);
    bufferfactor->setText("1");

    cpreset1->setAutoExclusive(false);
    cpreset2->setAutoExclusive(false);
    cpreset3->setAutoExclusive(false);

    cpreset1->setChecked(false);
    cpreset2->setChecked(false);
    cpreset3->setChecked(false);

    cpreset1->setAutoExclusive(true);
    cpreset2->setAutoExclusive(true);
    cpreset3->setAutoExclusive(true);
}

void cacheDialog::on_copyxml_clicked()
{
    QString line1;
    QString line2;
    QString line3;
    QString line4;
    QString line5;
    QString line6;
    QString line7;

   bool oldxml = xmlcheckBox->isChecked();

   QString buffersize = this->buffersize->text();
   int buffermode = this->buffermode->currentIndex();
   QString bufferfactor = this->bufferfactor->text();

    QString str1;
    str1.setNum(buffermode-1);

    if (oldxml)
      {
          line1 ="<advancedsettings>";
          line2 ="  <network>";
          line3 ="    <buffermode>"+str1+"</buffermode>";
          line4 ="    <cachemembuffersize>"+buffersize+"</cachemembuffersize>";
          line5 ="    <readbufferfactor>"+bufferfactor+"</readbufferfactor>";
          line6 ="  </network>";
          line7 ="</advancedsettings>";
      }

    else
      {
         line1 ="<advancedsettings>";
         line2 ="  <cache>";
         line3 ="    <buffermode>"+str1+"</buffermode>";
         line4 ="    <memorysize>"+buffersize+"</memorysize>";
         line5 ="    <readfactor>"+bufferfactor+"</readfactor>";
         line6 ="  </cache>";
         line7 ="</advancedsettings>";
      }

    QString cacheFilename = "temp.txt";
    QFile cacheFile(cacheFilename);
    cacheFile.open(QIODevice::ReadWrite);
    QTextStream out1(&cacheFile);

       out1  << line1 << Qt::endl;
       out1  << line2 << Qt::endl;
       out1  << line3 << Qt::endl;
       out1  << line4 << Qt::endl;
       out1  << line5 << Qt::endl;
       out1  << line6 << Qt::endl;
       out1  << line7 << Qt::endl;

    cacheFile.flush();
    out1.seek(0);
    QString temp = out1.readAll();
    cacheclipboard->setText(temp);
    cacheFile.close();
}
