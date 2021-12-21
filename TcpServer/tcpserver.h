#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDataStream>
namespace Ui {
class TcpServer;
}

class QTcpServer;
class QTcpSocket;

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = 0);
    ~TcpServer();

private slots:
    void newConnection(); // call for new client
    void removeConnection(); // call when a client wants to disconnect
    void readyRead(); // ready to send a message / receive a message
    void incrementCount(); // incrementing and decrementing a counter for # of players
    void decrementCount();

    void on_disconnectClients_clicked(); // call if the server decides to close (disconnect all)
    void sqlUpdate(int player, int amount);
    void sqlShowAll();
    void closeDatabase();
    void sqlDrop();

private:
    Ui::TcpServer *ui;
    QTcpServer *m_server; // making a TcpServer type variable called m_server
    int index; // used for # of players connected
    QString localhostIP;
    QTimer* timer;
    QVector<QTcpSocket*> m_clients; // used to keep track of connected clients and appropriately disconnect them
    QVector<qint32> fishesX;
    QVector<qint32> fishesY;
    QVector<qint32> endScores;
    QDataStream message;

    QHash<QTcpSocket*, QByteArray> m_receivedData; // Used to store received data

    QSqlDatabase db; // SQL database and tableview
    QSqlQueryModel* model;
    QByteArray x_arr;
    QByteArray y_arr;


    int player1score = 0;
    int player2score = 0;
    int player3score = 0;
    int player4score = 0;
    int u = 1;
    int counter = 0;
    int scorelist[4] = {player1score, player2score, player3score, player4score};

    void newMessage(QTcpSocket *sender, const QString& message); // called if the client wants to send a message
    void spawnFishCoords();
    void broadcast(QString data_string);
};

#endif // TCPSERVER_H
