#include "waittingDialog.h"
#include "ui_waittingDialog.h"

waittingDialog::waittingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::waittingDialog)
{
    ui->setupUi(this);
    this->setStyleSheet("outline: none");//delect all widget focus;
    setWindowFlags(Qt::FramelessWindowHint | this->windowFlags());
    movieStart();
    m_waitTimer = new QTimer;
    connect(m_waitTimer, SIGNAL(timeout()), this, SLOT(close()));
}

waittingDialog::waittingDialog(deviceTransceiver *devTranscviver, ProtocolType protocol, QWidget *parent) :
   waittingDialog(parent)
{
    m_protocolType = protocol;

    connect(devTranscviver, SIGNAL(readyRead(QByteArray)), this, SLOT(readDataSlot(QByteArray)));
}

waittingDialog::~waittingDialog()
{
    delete m_waitTimer;
    delete ui;
}

int waittingDialog::setProtocolType(waittingDialog::ProtocolType prototolType)
{
    m_protocolType = prototolType;

    return 0;
}

void waittingDialog::setMessage(QString message)
{
    ui->label->setText(message);
}

int waittingDialog::waitForClose(int msecs)
{
    m_readCache.clear();
    m_waitTimer->start(msecs);
    return this->exec();
}

void waittingDialog::acceptAtOnce()
{
    m_waitTimer->stop();
    this->accept();
}

void waittingDialog::paintEvent(QPaintEvent *)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    //QColor color("#ffff00");
    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - sqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

int waittingDialog::movieStart()
{
    movie.setFileName(":/img/gif/waitcircle.gif");
    ui->label_2->setMovie(&movie);
    QSize size(111,91);
    movie.setScaledSize(size);
    movie.start();

    return 0;
}

void waittingDialog::readDataSlot(QByteArray data)
{
    m_readCache.append(data);
    int ret = 0;
    switch (m_protocolType) {
    case PMbus:
    {
        PMBusProtocol pm;
        ret = pm.fromRawData(m_readCache.constData(), m_readCache.length());
        if ( PMBUS_RTN_NO_ERR == ret )
        {
            this->acceptAtOnce();
        }
    }
        break;
    case TP02:
    {
        TP02Protocol tp;
        ret = tp.fromRawData(m_readCache.constData(), m_readCache.length());
        if ( TP02_RTN_NO_ERR == ret )
        {
            this->acceptAtOnce();
        }
    }
        break;
    default:
        break;
    }
}
