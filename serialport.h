#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QWidget>
#include"global.h"
#include<QLabel>
#include<QWidget>
#include<QSpinBox>
#include<QTextEdit>
#include<QLineEdit>
#include<QCheckBox>
#include<QLCDNumber>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QVBoxLayout>
#include<QMessageBox>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<termios.h>
#include<QButtonGroup>
#include<QSocketNotifier>
#include<QScrollBar>
#include<QTimer>
#include<errno.h>



namespace Ui {
class serialport;
}

class serialport : public QWidget
{
    Q_OBJECT

public:
    explicit serialport(QWidget *parent = NULL);
    ~serialport();

public:
    void RefreshTheUSBList(void);
    void clearData();
    void sendData();

private:
    int fd;
    QString revData;
    QTimer *autoTimer;
    QSocketNotifier *monitor;

private slots:
    void serialRcvData();

    bool on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

    void on_pushButton_send_clicked();

    void on_comboBox_baudRate_currentIndexChanged(int index);

    void on_comboBox_dataBit_currentIndexChanged(int index);

    void on_comboBox_parityBit_currentIndexChanged(int index);

    void on_comboBox_stopBit_currentIndexChanged(int index);

    void on_pushButton_clear_clicked();

private:
    Ui::serialport *ui;
};

#endif // SERIALPORT_H
