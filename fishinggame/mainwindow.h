/*
 * Fishing Simulator 2021
 * CPE 353, Fall 2021
 * Made by:
 * Ben Bruzewski, Ben McAnulty, Jacob Smith, and Jesse Cowart
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QtGlobal>
#include <QtDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "player.h"
#include "defs.h"
#include "fish.h"
#include <QtNetwork>
#include <QLabel>
#include <QFont>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);



protected:
    void keyPressEvent(QKeyEvent *e); // event handler to utilize WASD/IJKL movement

private:
    Ui::MainWindow *ui;

    Player* player = nullptr;
    Fish* fish = nullptr;
    QLabel *label = new QLabel(this);
    QGraphicsScene* scene = nullptr;
    QGraphicsPixmapItem* pm1 = nullptr;
    QGraphicsPixmapItem* pm2 = nullptr;
    QGraphicsPixmapItem* pm3 = nullptr;
    QGraphicsPixmapItem* pm4 = nullptr;
    QTcpSocket *m_socket;

    int player1score = 0; // basic score tracker used for testing
    int player1scoreb = 0; // finalized scores which will be displayed at the end (pulled from tcp database)
    int player2scoreb = 0;
    int player3scoreb = 0;
    int player4scoreb = 0;
    int fishX; // fish positioning used for placement
    int fishY;
    int x;
    int y;


  //  void drawPerimeterLines();
    void spawnPlayer();
    void spawnFish(int x, int y);
    void spawnInitialFish();
    void drawPerimeterLines();
    void fishCollision(int hitX, int hitY);
    void joinServer(QString ip, int port, QTcpSocket*m_socket);

    QPixmap images[4] = { QPixmap(":/images/Bucket1.png"),
                          QPixmap(":/images/Bucket2.png"),
                          QPixmap(":/images/Bucket3.png"),
                          QPixmap(":/images/Bucket4.png")};

    int loadingindex;
    int loadingtimer = 5;
    int client_num = 1; // identifies which player each client code is. used for ordering on scoreboard
    int x_hit;
    int y_hit;
    QString cn;

    QTimer *timer;              // Timer Stuff - create a new timer
    int timercountdown = 120;    // Timer Stuff - variable for timer countdown
        // timercountdown needs be be 120 for a two minute game
signals:
    void stop();
    void ready();
    void sendPlayer(int client_num);

private slots:
    void joinMenu();
    void InGameTime();
    void UpdateTimer(); // Timer Stuff
    void loadingScreen();
    void joinGame();
    void quitMenu();
    void helpMenu();
    void mainMenu();
    void readyRead();
    void returntogame();
    void quittoMain();
    void quittoDesk();
    void hardquit();
    void UpdateScoreBoard();
    void returntoMenu();
    void startGame();
    //void UpdateTimer2(); // Loading Timer Stuff
    //void loadingGame();
};

#endif // MAINWINDOW_H
