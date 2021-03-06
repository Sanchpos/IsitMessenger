#include "LocalClient.hpp"

#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>

LocalClient::LocalClient(QObject *parent) :
    QObject(parent),
    m_socket(new QTcpSocket(this))
{
    connect(m_socket, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));

    connect(m_socket, SIGNAL(connected()),this, SLOT(onConnected()));
}

void LocalClient::connectToServer(QString address, quint16 port)
{
    m_socket->connectToHost(address, port);
}

void LocalClient::sendMessage(QString message)
{
    m_socket->write("m:");
    m_socket->write(message.toUtf8());
}

void LocalClient::setNickname(QString nickname)
{
    m_socket->write("setNickname:" + nickname.toUtf8());
    m_pendingNickname = nickname;
}

void LocalClient::getParticipants()
{
    m_socket->write("getParticipants()");
}

void LocalClient::sendTunneledMessage(int idTo, const QByteArray &message)
{
    m_socket->write("Tunnel:" + QString::number(idTo).toLocal8Bit() + ":" + message);
}

void LocalClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();


    if (data.startsWith("Tunnel:")){
        QList<QByteArray> dat = data.split(':');
        emit onTunneledMessageReceived(dat[1].toInt(), dat[2]);
    }
    if (data.startsWith("m:")){
        emit messageReceived("123", data.mid(2, -1));
    }
    if(data.startsWith("NicknameStatus:0")){

    }
    if(data.startsWith("NicknameStatus:1")){
        m_nickname = m_pendingNickname;
    }
    if (data.startsWith("Participants:")){
        data = data.mid(13);
        QDataStream stream(data);
        QList <int> ids;
        QStringList names;
        int count = 0;
        stream >> count;
        for (int i = 0; i < count; ++i){
            int id;
            QString name;
            stream >> id >> name;
            ids.append(id);
            names.append(name);
        }
        emit participantsReceived(ids, names);
    }
}

void LocalClient::onConnected()
{
    getParticipants();
    emit connected();
}
