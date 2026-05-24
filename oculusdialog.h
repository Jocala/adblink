#ifndef OCULUSDIALOG_H
#define OCULUSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

class oculusDialog : public QDialog
{
    Q_OBJECT

public:
    explicit oculusDialog(QWidget *parent = nullptr);
    ~oculusDialog();

    void setbattery1label(const QString &b1);

    int  powerSelected() const;
    int  proximitySelected() const;
    int  guardianSelected() const;
    int  cpuSelected() const;
    int  gpuSelected() const;
    int  refreshSelected() const;

    int  textureSelected() const;
    int  recordingSelected() const;
    int  fovlevelSelected() const;
    int  dynamicfovSelected() const;
    int  ratecapSelected() const;
    int  chromaticSelected() const;
    int  experSelected() const;

    QString  bitrateSelected() const;

    QString fovdownSelected() const;
    QString fovupSelected() const;
    QString fovoutSelected() const;
    QString fovinSelected() const;

    void fovdownSet(QString fovdown);
    void fovupSet(QString fovup);
    void fovoutSet(QString fovout);
    void fovinSet(QString fovin);

    void  powerSet(int index);
    void  proximitySet(int index);
    void  guardianSet(int index);
    void  cpuSet(int index);
    void  gpuSet(int index);
    void  refreshSet(int index);
    void  experSet(int index);

    void  textureSet(int index);
    void  recordingSet(int index);
    void  fovlevelSet(int index);
    void  dynamicfovSet(int index);
    void  ratecapSet(int index);
    void  chromaticSet(int index);

    void  customCaptureWidthSet(QString customCaptureWidth);
    void  customCaptureHeightSet(QString customCaptureHeight);

    QString  customCaptureHeightSelected() const;
    QString  customCaptureWidthSelected() const;

    QString  customTextureHeightSelected() const;
    QString  customTextureWidthSelected() const;

    void  customTextureWidthSet(QString customTextureWidth);
    void  customTextureHeightSet(QString customTextureHeight);

    void  bitrateSet(QString customBitrate);
    void  titleSet(QString title);

    int clickedButton;

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();
    void on_okButton_clicked();

private:
    QLabel *title;
    QLabel *battery1Label;
    QComboBox *powerBox;
    QComboBox *proximityBox;
    QComboBox *guardianBox;
    QComboBox *cpulevelBox;
    QComboBox *gpulevelBox;
    QComboBox *refreshBox;
    QComboBox *experBox;
    QComboBox *textureBox;
    QComboBox *resolutionBox;
    QComboBox *foveation1Box;
    QComboBox *foveation2Box;
    QComboBox *ratecaptureBox;
    QComboBox *chromaticBox;
    QLineEdit *fovdown;
    QLineEdit *fovup;
    QLineEdit *fovout;
    QLineEdit *fovin;
    QLineEdit *captureWidth;
    QLineEdit *captureHeight;
    QLineEdit *textureWidth;
    QLineEdit *textureHeight;
    QLineEdit *captureBitrate;
    QPushButton *cancelButton;
    QPushButton *saveButton;
    QPushButton *okButton;
};

#endif // OCULUSDIALOG_H
