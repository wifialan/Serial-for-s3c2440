#include "serialport.h"
#include "ui_serialport.h"


serialport::serialport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::serialport)
{
    ui->setupUi(this);
    //serial = new QSerialPort();
    fd = -1;

    //ui default config
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
    ui->pushButton_send->setEnabled(true);
    ui->comboBox_serialPortDevice->setCurrentIndex(0);
    ui->comboBox_baudRate->setCurrentIndex(CONFIG_BAUDRATE_115200_INDEX);
    ui->comboBox_dataBit->setCurrentIndex(CONFIG_DATABITS_8_INDEX);
    ui->comboBox_parityBit->setCurrentIndex(CONFIG_PARITY_NONE_INDEX);
    ui->comboBox_stopBit->setCurrentIndex(CONFIG_STOPBIT_ONE_INDEX);

}

serialport::~serialport()
{
    delete ui;
}

void serialport::RefreshTheUSBList(void)
{
    QString protName;
    QString uartName;
    //ui->comboBox_serialPortDevice->clear();


}

bool serialport::on_pushButton_connect_clicked()
{
    struct termios setting;
    fd = open(qPrintable("/dev/ttyUSB0"),O_RDWR);
    if (fd == -1){
        QMessageBox::warning(this, "open error", strerror(errno));
        goto openerr;
    }
    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
    tcgetattr(fd,&setting);
    speed_t speed;
    //Setting BaudRate
    switch (ui->comboBox_baudRate->currentIndex()) {
    case CONFIG_BAUDRATE_115200_INDEX:speed = B115200;break;
    case CONFIG_BAUDRATE_57600_INDEX:speed = B57600;break;
    case CONFIG_BAUDRATE_38400_INDEX:speed = B38400;break;
    case CONFIG_BAUDRATE_19200_INDEX:speed = B19200;break;
    case CONFIG_BAUDRATE_9600_INDEX:speed = B9600;break;
    case CONFIG_BAUDRATE_4800_INDEX:speed = B4800;break;
    case CONFIG_BAUDRATE_1200_INDEX:speed = B1200;break;
        //    default:speed = B115200;break;
//    default:goto seterr;
    }
    cfsetispeed(&setting,speed);
    cfsetospeed(&setting,speed);
    cfmakeraw(&setting);
    //Setting DataBit
    switch (ui->comboBox_dataBit->currentIndex()) {
    case CONFIG_DATABITS_8_INDEX: setting.c_cflag |= CS8;break;
    case CONFIG_DATABITS_7_INDEX: setting.c_cflag |= CS7;break;
    case CONFIG_DATABITS_6_INDEX: setting.c_cflag |= CS6;break;
    case CONFIG_DATABITS_5_INDEX: setting.c_cflag |= CS5;break;
        //    default: setting.c_cflag |= CS8;break;
//    default:goto seterr;
    }
    //Setting ParityBit
    switch (ui->comboBox_parityBit->currentIndex()) {
    case CONFIG_PARITY_NONE_INDEX: setting.c_cflag &= ~PARENB;setting.c_iflag &= ~INPCK;break;
    case CONFIG_PARITY_EVEN_INDEX: setting.c_cflag |= PARENB;setting.c_cflag &= ~PARODD;setting.c_iflag |= INPCK;break;
    case CONFIG_PARITY_ODD_INDEX: setting.c_cflag |= (PARODD | PARENB);setting.c_iflag |= INPCK;break;
        //    default: setting.c_cflag &= ~PARENB;setting.c_iflag &= ~INPCK;break;
//    default:goto seterr;
    }
    //Setting StopBit
    switch (ui->comboBox_stopBit->currentIndex()) {
    case CONFIG_STOPBIT_ONE_INDEX: setting.c_cflag &= ~CSTOPB;break;
    case CONFIG_STOPBIT_TWO_INDEX: setting.c_cflag |= CSTOPB;break;
        //    default: setting.c_cflag |= CSTOPB;break;
//    default:goto seterr;
    }
    tcflush(fd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &setting);
    monitor = new QSocketNotifier(fd, QSocketNotifier::Read, NULL);
    autoTimer = new QTimer;
    connect(monitor, SIGNAL(activated(int)), this, SLOT( serialRcvData()) );
    connect(autoTimer,SIGNAL(timeout()),this, SLOT( on_pushButton_send_clicked() ));
    monitor->setEnabled(true);
    //    charBox->setCheckState(Qt::Checked);
    //    autoSend->setCheckState(Qt::Unchecked);
    //    sendSpin->setValue(100);
    //    sendInfo->clear();
    return true;
seterr:
    ::close(fd);
openerr:
    return false;
}

void serialport::serialRcvData()
{
    char buf[2000];
    int ret;
    int i;
    while (revData.size() > 9999) {
        revData.remove(0,1000);
    }
    ret = read(fd,buf,sizeof(buf));
    revData = QByteArray::fromRawData(buf,ret);
    //    ui->textBrowser_receiveData->append(revData);
    ui->textBrowser_receiveData->insertPlainText(revData);

}


void serialport::on_pushButton_disconnect_clicked()
{
    ::close(fd);
    autoTimer->stop();
    delete monitor;
    delete autoTimer;
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
}

void serialport::on_pushButton_send_clicked()
{
    qint8 ret;
    QString input = ui->textEdit_send->toPlainText();
//    QByteArray temp;
    input.append("Hello World");

    if (input.isEmpty() == true){
        return;
    } else {
        ret = write(fd,input.toLatin1(),input.size());

    }
}

void serialport::on_comboBox_baudRate_currentIndexChanged(int index)
{
    struct termios setting;
    tcgetattr(fd,&setting);
    speed_t speed;
    //Setting BaudRate
    switch (index) {
    case CONFIG_BAUDRATE_115200_INDEX:speed = B115200;break;
    case CONFIG_BAUDRATE_57600_INDEX:speed = B57600;break;
    case CONFIG_BAUDRATE_38400_INDEX:speed = B38400;break;
    case CONFIG_BAUDRATE_19200_INDEX:speed = B19200;break;
    case CONFIG_BAUDRATE_9600_INDEX:speed = B9600;break;
    case CONFIG_BAUDRATE_4800_INDEX:speed = B4800;break;
    case CONFIG_BAUDRATE_1200_INDEX:speed = B1200;break;
    }
    cfsetispeed(&setting,speed);
    cfsetospeed(&setting,speed);
    cfmakeraw(&setting);
    tcflush(fd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &setting);
}

void serialport::on_comboBox_dataBit_currentIndexChanged(int index)
{
    struct termios setting;
    tcgetattr(fd,&setting);
    //Setting DataBit
    switch (index) {
    case CONFIG_DATABITS_8_INDEX: setting.c_cflag |= CS8;break;
    case CONFIG_DATABITS_7_INDEX: setting.c_cflag |= CS7;break;
    case CONFIG_DATABITS_6_INDEX: setting.c_cflag |= CS6;break;
    case CONFIG_DATABITS_5_INDEX: setting.c_cflag |= CS5;break;
        //    default: setting.c_cflag |= CS8;break;
    }
    tcflush(fd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &setting);
}

void serialport::on_comboBox_parityBit_currentIndexChanged(int index)
{
    struct termios setting;
    tcgetattr(fd,&setting);
    //Setting ParityBit
    switch (index) {
    case CONFIG_PARITY_NONE_INDEX: setting.c_cflag &= ~PARENB;setting.c_iflag &= ~INPCK;break;
    case CONFIG_PARITY_EVEN_INDEX: setting.c_cflag |= PARENB;setting.c_cflag &= ~PARODD;setting.c_iflag |= INPCK;break;
    case CONFIG_PARITY_ODD_INDEX: setting.c_cflag |= (PARODD | PARENB);setting.c_iflag |= INPCK;break;
    }
    tcflush(fd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &setting);
}

void serialport::on_comboBox_stopBit_currentIndexChanged(int index)
{
    struct termios setting;
    tcgetattr(fd,&setting);
    //Setting StopBit
    switch (index) {
    case CONFIG_STOPBIT_ONE_INDEX: setting.c_cflag &= ~CSTOPB;break;
    case CONFIG_STOPBIT_TWO_INDEX: setting.c_cflag |= CSTOPB;break;
    }
    tcflush(fd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &setting);
}

void serialport::on_pushButton_clear_clicked()
{
    ui->textBrowser_receiveData->clear();
}

