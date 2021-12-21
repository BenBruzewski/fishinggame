#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "defs.h"

#include <QtNetwork>
#include <QPlainTextEdit>
#include <QNetworkProxy>
#include <QSqlQuery>
#include <QString>

// may need more later for player data, etc...

// Need to modify functionality to deliver player pos to each client and then update those positions (current functionality is QString type msgs)
// Every 5ms, 10ms? whatever Qt can handle really.

TcpServer::TcpServer(QWidget *parent) : QWidget(parent), ui(new Ui::TcpServer), m_server(new QTcpServer(this))
{
    ui->setupUi(this);
    QString localhostname =  QHostInfo::localHostName();

   QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
   foreach (const QHostAddress& address, hostList) {
       if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false) {
            localhostIP = address.toString();
       }
   }
   qDebug() << "Localhost name: " << localhostname;
   qDebug() << "IP = " << localhostIP;
    if (!m_server->listen(QHostAddress(localhostIP), 52693)) { // if the server isn't listening ...
        ui->log->setPlainText(tr("Failure while starting server: %1").arg(m_server->errorString()));
        return; // let the user know of the error.
    }
    qDebug() << "IsListening: " << m_server->isListening();

    m_server->setMaxPendingConnections(4); // 4 connections at a time
    index = 0; // count # of connections.
    ui->lcdNumber->display(0); // make sure default value is 0.

    connect(m_server, &QTcpServer::newConnection,
            this, &TcpServer::newConnection); // when a new connection is detected, use the newconnection function
    ui->address->setText(m_server->serverAddress().toString()); // displaying ADR and Port # for convenience
    ui->port->setText(QString::number(m_server->serverPort()));

    // Starting SQL database here:
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory"); // :memory is a special name that defines the database for this session ONLY! if we want a long-term database change this!
        // should work fine since the server will be open for the whole game, and we're not planning to save "high scores" so.

    if (!db.open()) // if the database isn't open by this point, we have a problem...
    {
        qDebug() << db.lastError();
        qDebug() << "Error: unable to connect due to above error.";
        return;
    }

    QSqlQuery q;
    q.prepare("CREATE TABLE IF NOT EXISTS scores (id INT, points INT)");
    if (!q.exec())
        qDebug() << q.lastError();
    else {
        qDebug() << "Table successfully generated.";
    }

    q.prepare("INSERT INTO scores (id, points) VALUES (1, :p1)");
    q.bindValue(":p1", player1score);
    q.exec();
    q.prepare("INSERT INTO scores (id, points) VALUES (2, :p2)");
    q.bindValue(":p2", player2score);
    q.exec();
    q.prepare("INSERT INTO scores (id, points) VALUES (3, :p3)");
    q.bindValue(":p3", player3score);
    q.exec();
    q.prepare("INSERT INTO scores (id, points) VALUES (4, :p4)");
    q.bindValue(":p4", player4score);
    q.exec();
    // to use variables we must bind them first, so we'll use bindvalue

    // emptying the table inside closeDatabase() now. Please only call this once the scores have been updated, that way we can see them in the server. Thanks!
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::newConnection()
{
    qDebug() << "Received new connection...";
    if (index == 4) // if 4 people have joined...
    {
        qDebug() << "Server is full!";
//        for (int i = 0; i < m_clients.length(); i++)
//        {
//            m_clients[i]->write("START");
//            m_clients[i]->flush();
//        }
        //m_server->close(); // no more are allowed to join
//        connect(m_server, &QTcpServer::
    }
    else if (index <= 4) // if LT 4 ...
    {
    incrementCount(); // go ahead and let them join


    while (m_server->hasPendingConnections()) { // while people want to join...
        QTcpSocket *socket = m_server->nextPendingConnection(); // go ahead and get the next connection in line as a socket
        socket->setProxy(QNetworkProxy::NoProxy);
        m_clients << socket; // add the socket to our clients list
        ui->disconnectClients->setEnabled(true); // now our server can disconnect everyone
        connect(socket, &QTcpSocket::disconnected,
                this, &TcpServer::removeConnection); // if a client leaves, call removeconnection
        connect(socket, &QTcpSocket::readyRead,
                this, &TcpServer::readyRead); // if thesockert wants to send data, call readyread.
        ui->log->appendPlainText(tr("* New connection: %1, port %2")
                                 .arg(socket->peerAddress().toString())
                                 .arg(socket->peerPort())); // displaying a notification when someone joins. ADR, Port.
        qDebug() << "Server socket is: " << socket->state();

        foreach (QTcpSocket *socket, m_clients)
        {
            QString newConn = "Server received new connection!\n";
            QByteArray messageArray = newConn.toUtf8();
            socket->write(messageArray, messageArray.length());
        }
        if (index == 4) // if 4 people have joined...
        {
            qDebug() << "Server is full. Ready to start match.";
//            for (int i = 0; i < 8; i++)
//            {
//                qint32 fishLocX = QRandomGenerator::global()->bounded(-SCENE_WIDTH/2 + 10,SCENE_WIDTH/2 - 10);// go ahead and generate a new X and Y for the next fish...
//                qint32 fishLocY = QRandomGenerator::global()->bounded(-SCENE_HEIGHT/2 + 10,SCENE_HEIGHT/2 - 10);
//            }
            foreach (QTcpSocket *socket, m_clients)
            {
                QString newConn = "START\n";
                QByteArray messageArray = newConn.toUtf8();
                socket->write(messageArray);

                QString playernum = QString::number(u);
                QByteArray pnArr = playernum.toUtf8();
                socket->write("pn: " + pnArr + "\n");
                u++;
                for (int i = 0; i < 8; i++)
                {
                    qint32 x = QRandomGenerator::global()->bounded(-SCENE_WIDTH/2 + 10,SCENE_WIDTH/2 - 10); // go ahead and generate a new X and Y for the next fish...
                    QString x_str = QString::number(x);
                    //qDebug() << "x: " << x_str;
                    QByteArray xArray = x_str.toUtf8();
                    socket->write("x: " +xArray + "\n");
                    qint32 y = QRandomGenerator::global()->bounded(-SCENE_HEIGHT/2 + 10,SCENE_HEIGHT/2 - 10);
                    QString y_str = QString::number(y);
                    QByteArray yArray = y_str.toUtf8();
                    socket->write("y: " +yArray + "\n");
                }
            }
        }
    }
    }
}

void TcpServer::removeConnection()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    // sender is useful here to make sure we're looking at the right client before we make a horrible mistake.

    if (!socket) { // if socket == 0 AKA client is gone...
        return;
    }
    ui->log->appendPlainText(tr("* Connection removed: %1, port %2") // telling the log a user left (ADR, port)
                             .arg(socket->peerAddress().toString())
                             .arg(socket->peerPort()));
    m_clients.removeOne(socket); // now we're removing the client from the clientlist and the dataset
    m_receivedData.remove(socket);
    socket->deleteLater(); // clean up the memory later
    ui->disconnectClients->setEnabled(!m_clients.isEmpty()); // server can still DC clients if someone is here.

    decrementCount(); // decrementing our LCD counter
    if (index < 4 && (!m_server->isListening())) // if there's less than 4 people and the server closed, open it!
        m_server->listen(QHostAddress(localhostIP), 52693); // reopening server
}

void TcpServer::readyRead()
{
    //qDebug() << "In ReadyRead! ";
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender()); // same as removeconnection, make sure we have the right client.
    QByteArray &buffer = m_receivedData[socket];
    buffer.append(socket->readLine());
    socket->setProxy(QNetworkProxy::NoProxy);
    if (!socket) { // if the client is gone, don't move on.
        return;
      }
    QByteArray data = m_receivedData[socket];
    QString data_string =QString::fromStdString(data.toStdString());
    data_string = data_string.simplified();

    qDebug() << "data string: " << data_string;

    if (data_string.left(3) == "go:")
    {
        int player = data_string.mid(3,1).toInt();
        qDebug() << "player: " << player;
        int score = data_string.mid(5).toInt();
        qDebug() << "score: " << score;
        counter++;
        sqlUpdate(player, score);
    }

    if (data_string.left(2) == "b:")
    {
        broadcast(data_string);
    }
    //qDebug() << data_string;
}

//    QByteArray &buffer = m_receivedData[socket]; // putting the client data into a bytearray
//    buffer.append(socket->readAll()); // read the whole message
//    while(true) {
//        int endIndex = buffer.indexOf(23); // 23 = end of transmission block in ASCII
//        if (endIndex < 0) { // if we hit the end of the message....
//            break;
//        }
//        QString message = QString::fromUtf8(buffer.left(endIndex)); // convert the message to a string for display
//        buffer.remove(0, endIndex + 1); // recommended practice online to move msg out of buffer
//        newMessage(socket, message); // display the message with newmessage()
//    }

void TcpServer::spawnFishCoords()
{
    qint32 fishLocX = QRandomGenerator::global()->bounded(-SCENE_WIDTH/2 + 10,SCENE_WIDTH/2 - 10);// go ahead and generate a new X and Y for the next fish...
    qint32 fishLocY = QRandomGenerator::global()->bounded(-SCENE_HEIGHT/2 + 10,SCENE_HEIGHT/2 - 10);
    fishesX.push_back(fishLocX);
    fishesY.push_back(fishLocY);
}
void TcpServer::on_disconnectClients_clicked()
{
    foreach (QTcpSocket *socket, m_clients) { // for every client still connected
        socket->close(); // close them out
    }
    ui->disconnectClients->setEnabled(false); // remove disconnect all button functionality since they all got disconnected.
}

void TcpServer::newMessage(QTcpSocket *sender, const QString &message)
{
    ui->log->appendPlainText(tr("Sending message: %1").arg(message)); // put the message in the log
    QByteArray messageArray = message.toUtf8(); // conv. QString to QByteArray w/ UTF-8
    messageArray.append(23); // append the separator char to end of msg (end char)
    for(QTcpSocket *socket: m_clients) { // check which clients are still here and send it to them all
        if (socket->state() == QAbstractSocket::ConnectedState) { // if they're here.
            socket->write(messageArray); // send it to that particular client
        } // * could be modified to send player data later!
    }
    Q_UNUSED(sender) // not using sender() here, Q_UNUSED removes sender warnings since its a parameter.
}

void TcpServer::broadcast(QString data_string)
{
    QByteArray data_array = data_string.toUtf8();
    foreach (QTcpSocket *socket, m_clients) {
        socket->write(data_array);
    }
}

void TcpServer::incrementCount()
{
    index++; // incrementin    sqlDrop(); // delete the table once we're done since we don't want it lingering between game sessions...g the player count
    ui->lcdNumber->display(index); // push this new vlaue to our display for the server
} // Ultimately we should move the server into the main code as a host, and the client code as a player.

void TcpServer::decrementCount()
{
    index--;
    ui->lcdNumber->display(index);
}

void TcpServer::sqlUpdate(int player, int amount)
{
    QSqlQuery q;
    scorelist[player-1] = scorelist[player-1] + amount;

    q.prepare("UPDATE scores SET points=:points WHERE id=:id");
    q.bindValue(":points", scorelist[player-1]);
    q.bindValue(":id", player);

    if (!q.exec()) // this actually executes the command so keep it!
    {
        qDebug() << "ERROR executing insert: " << q.lastError();
        //return;
    }
    else
    {
//        ui->tableView->setModel(model); // update the table contents in the GUI
        qDebug() << "UPDATE successful!";
    }

    if (counter == 4)
    {
        model = new QSqlQueryModel; // lets us read the data in our database. maybe we use this for the scoreboard? or move this to the game code..
        model->setQuery("SELECT * FROM scores");
        ui->tableView->setModel(model);
        q.exec("SELECT points FROM scores WHERE id=1");
        q.next();
        int p1score = q.value(0).toInt();
        qDebug() << "p1: " << p1score;
        q.exec("SELECT points FROM scores WHERE id=2");
        q.next();
        int p2score = q.value(0).toInt();
        qDebug() << "p2: " << p2score;
        q.exec("SELECT points FROM scores WHERE id=3");
        q.next();
        int p3score = q.value(0).toInt();
        qDebug() << "p3: " << p3score;
        q.exec("SELECT points FROM scores WHERE id=4");
        q.next();
        int p4score = q.value(0).toInt();
        qDebug() << "p4: " << p4score;

        QString p1score_str = QString::number(p1score);
        QString p2score_str = QString::number(p2score);
        QString p3score_str = QString::number(p3score);
        QString p4score_str = QString::number(p4score);

        QByteArray p1send = p1score_str.toUtf8();
        QByteArray p2send = p2score_str.toUtf8();
        QByteArray p3send = p3score_str.toUtf8();
        QByteArray p4send = p4score_str.toUtf8();

        foreach (QTcpSocket *socket, m_clients) {
            socket->write("sb1:" +p1send +"\n");
            socket->write("sb2:" +p2send +"\n");
            socket->write("sb3:" +p3send +"\n");
            socket->write("sb4:" +p4send +"\n");
        }

        closeDatabase();


    }
}

void TcpServer::sqlShowAll()
{
    model->setQuery("SELECT * FROM scores");
}

void TcpServer::closeDatabase()
{
    QSqlQuery q;
    if (!q.exec("DELETE FROM scores")) // THIS IS IT! USE THIS TO CLEAR A TABLE
        qDebug() << "Delete command failed b/c " << q.lastError();
    qDebug() << "Entered CloseDatabase";
    db.close();
}

void TcpServer::sqlDrop()
{
    // deletes the database
    QSqlDatabase::removeDatabase("scores");
    //model->setQuery("DROP TABLE scores");
    //ui->tableView->setModel(model);

}
