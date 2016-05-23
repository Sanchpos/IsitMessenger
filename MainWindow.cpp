#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "Server.hpp"
#include "ServerDiscovery.hpp"
#include "LocalClient.hpp"

GomokuWindow::GomokuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_client(new LocalClient(this))
{
    ui->setupUi(this);

    connect(ui->createServer, SIGNAL(clicked(bool)),
            this, SLOT(createServer()));

    connect(ui->message, SIGNAL(returnPressed()),
            this, SLOT(sendMessage()));

    connect(ui->sendButton, SIGNAL(clicked(bool)),
            this, SLOT(sendMessage()));

    m_serverDiscovery = new ServerDiscovery(this);
    m_server = new Server(this);

    connect(m_server, SIGNAL(serverStarted(quint16)),
            this, SLOT(addServerToDiscovery(quint16)));

    connect(m_server, SIGNAL(messageReceived(QString,QString)),
            this, SLOT(onServerMessageReceived(QString,QString)));

    connect(m_serverDiscovery,SIGNAL(serverFound(QString,quint16)),
            this, SLOT(addServer(QString,quint16)));

    connect(m_client, SIGNAL(messageReceived(QString,QString)),
            this, SLOT(addMessage(QString,QString)));

    connect(m_client, SIGNAL(connected()),
            this, SLOT(onClientConnected()));

    connect(m_client, SIGNAL(participantsReceived(QList<int>,QStringList)),
            this, SLOT(participantsOnReceived(QList<int>,QStringList)));

    m_serverDiscovery->discoveryServer();
}

GomokuWindow::~GomokuWindow()
{
    delete ui;
}

void GomokuWindow::connectToServer()
{
    m_client->connectToServer(ui->serverListAddress->text(),
                              ui->serverListPort->value());
}

void GomokuWindow::addContact(QString name)
{
    for(int i=0;i<ui->contacts->count();i++){
        QListWidgetItem *item = ui->contacts->item(i);
        if(item->text() == name){
            return;
        }
    }
    ui->contacts->addItem(name);
}

void GomokuWindow::addMessage(QString name, QString text)
{
    if (text.startsWith("/me ")){
        ui->messages->setFontItalic(true);
        ui->messages->append ('*' + name + ' ' + text.section(' ', 1, -1));
        return;
    }
    ui->messages->setFontItalic(false);
    ui->messages->append(name+":"+text+"\n");
}

void GomokuWindow::addServer(QString address, quint16 port)
{
    QString text = address+":"+QString::number(port);
    for(int i=0;i<ui->serverListList->count();i++){
        QListWidgetItem *item = ui->serverListList->item(i);
        if(item->text() == text){
            return;
        }
    }
    ui->serverListList->addItem(text);
}

void GomokuWindow::addServerToDiscovery(quint16 port)
{
    m_serverDiscovery->addServer(port);
}

void GomokuWindow::createServer()
{
    m_server->startServer(ui->serverPort->value());
    ui->createServer->setDisabled(true);
}

void GomokuWindow::participantsOnReceived(const QList<int> &ids, const QStringList &names)
{
    ui->contacts->clear();
    ui->contacts->addItems(names);
}

void GomokuWindow::onServerMessageReceived(QString from, QString message)
{
    ui->serverLog->insertPlainText(from + ":" + message + "\n");
}

void GomokuWindow::on_serverListConnectButton_clicked()
{
    connectToServer();
}

void GomokuWindow::on_serverListList_itemDoubleClicked(QListWidgetItem *item)
{
    connectToServer();
}

void GomokuWindow::on_serverListList_itemClicked(QListWidgetItem *item)
{
    ui->serverListAddress->setText(item->text().section(':',-2,-2));
    ui->serverListPort->setValue(item->text().section(':',-1).toInt());
}

void GomokuWindow::on_serverListRefreshButton_clicked()
{
    ui->serverListList->clear();
    m_serverDiscovery->discoveryServer();
}

void GomokuWindow::sendMessage()
{
    QString text = ui->message->text();
    ui->message->clear();
    if(text.startsWith("/nick ")){
        m_client->setNickname(text.mid(6, -1));
    } else {
        m_client->sendMessage(text);
    }
}

void GomokuWindow::onClientConnected()
{
    ui->tabWidget->setCurrentIndex(1);
    ui->messages->append("You connected to server \n");
}

void GomokuWindow::on_contacts_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    menu.exec(ui->contacts->mapToGlobal(pos));
}
