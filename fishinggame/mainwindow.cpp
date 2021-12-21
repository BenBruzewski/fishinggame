/*
 * Fishing Simulator 2021
 * CPE 353, Fall 2021
 * Made by:
 * Ben Bruzewski, Ben McAnulty, Jacob Smith, and Jesse Cowart
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defs.h"
#include "fish.h"
#include "player.h"
#include <QApplication>
#include <QDialog>
#include <QtWidgets>
#include <QString>
#include <QDebug>
#include <QGraphicsView>
#include <QtNetwork>
#include <QLabel>

static int curIndex; // used to move between pages of the game manually.

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->joinButton, SIGNAL(clicked()), this, SLOT(joinMenu()));
    connect(ui->quitdesktopButton, SIGNAL(clicked()), this, SLOT(quittoDesk()));
    connect(ui->helpButton2, SIGNAL(clicked()), this, SLOT(helpMenu()));
    connect(ui->mainmenuButton, SIGNAL(clicked()), this, SLOT(quittoMain()));
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(helpMenu())); // connecting our buttons to various functions
    ui->stackedWidget->setCurrentIndex(0); // start the game at the main menu (page 0)

    scene = new QGraphicsScene(-100,-100,200,200);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setBackgroundBrush(Qt::transparent);

    for (int k = 0; k < 4; k++)
        images[k] = images[k].scaled(50,50);
    ui->score->setAttribute(Qt::WA_TranslucentBackground);
    ui->score_2->setAttribute(Qt::WA_TranslucentBackground);
    ui->score_3->setAttribute(Qt::WA_TranslucentBackground);
    ui->score_4->setAttribute(Qt::WA_TranslucentBackground);

    connect(this, SIGNAL(ready()), this, SLOT(startGame())); // emits once 4 clients are connected
    drawPerimeterLines();

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    m_socket = new QTcpSocket(this);
    m_socket->setProxy(QNetworkProxy::NoProxy);
    connect(m_socket, &QTcpSocket::readyRead,
            this, &MainWindow::readyRead);          // When there's data ready to read, this slot will be called
}


void MainWindow::drawPerimeterLines()
{
    // Define perimeter of scene with four QGraphicsLineItem objects
    int w = SCENE_WIDTH;    // Define w,h shortcuts names
    int h = SCENE_HEIGHT;

    QGraphicsLineItem* topline = scene->addLine(-w/2,-h/2,w/2,-h/2);
    topline->setPen(QPen(Qt::blue));

    QGraphicsLineItem* bottomline = scene->addLine( -w/2,h/2,w/2,h/2);
    bottomline->setPen(QPen(Qt::blue));

    QGraphicsLineItem* leftline = scene->addLine(-w/2, -h/2,-w/2,h/2);
    leftline->setPen(QPen(Qt::blue));

    QGraphicsLineItem* rightline = scene->addLine(w/2,-h/2,w/2,h/2);
    rightline->setPen(QPen(Qt::blue));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event)
    {
        switch(event->key())
        {
        case Qt::Key_F2: // F1 and F2 manually move the client between pages. good for debugging and general use.
                                curIndex = (ui->stackedWidget->currentIndex()+1) % indexes;
                                qDebug() << curIndex;
                                ui->stackedWidget->setCurrentIndex(curIndex);
                                 break;
        case Qt::Key_F1:
                                curIndex = (ui->stackedWidget->currentIndex()-1 % indexes);
                                qDebug() << curIndex;
                                ui->stackedWidget->setCurrentIndex(curIndex);
                                break;
        }
    }

    curIndex = ui->stackedWidget->currentIndex();
    if(curIndex == gameplay) // if the game is on the game page, enable movement keys
    {
        if (event)
        {
            switch (event->key())
            {
                case Qt::Key_A:
                case Qt::Key_J:
                                        player->goLeft();
                                        break;

                case Qt::Key_D:
                case Qt::Key_L:

                                        player->goRight();
                                        break;

                case Qt::Key_W:
                case Qt::Key_I:
                                        player->goUp();
                                        break;

                case Qt::Key_S:
                case Qt::Key_K:
                                        player->goDown();
                                        break;

                case Qt::Key_Space:     player->stop(); // manually stop player with spacebar
                                        break;

                case Qt::Key_Escape:
                                        quitMenu(); // open a quit/help menu with ESC
                                        break;
                default:
                                        break;
            }
        }
    }
    QMainWindow::keyPressEvent(event);
}


void MainWindow::joinMenu()
{
    ui->stackedWidget->setCurrentIndex(1);

    QString localhostname =  QHostInfo::localHostName(); // grab the local host name and put it into a list
    QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
    foreach (const QHostAddress& address, hostList) { // for every item in the host list, confirm IPv4 and not a loopback adr, then put in the GUI
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false) {
             ui->ipEdit->setText(address.toString()); // set default values for the join menu to save players time.
        }
    }
      ui->portEdit->setText("52693"); // set a default port, but this can be altered if desired
      connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(mainMenu()));
      connect(ui->join2Button, SIGNAL(clicked()), this, SLOT(loadingScreen()));
}

void MainWindow::quitMenu() // this menu holds help options as well as quit button
{
    ui->stackedWidget->setCurrentIndex(quit);
    connect(ui->returnButton, SIGNAL(clicked()), this, SLOT(returntogame()));
}

void MainWindow::quittoDesk() // requests confirmation, then either closes program or returns to ESC menu
{
    ui->stackedWidget->setCurrentIndex(quitConfirm);
    connect(ui->yesButton, SIGNAL(clicked()), this, SLOT(hardquit()));
    player->despawn();
    connect(ui->noButton, SIGNAL(clicked()), this, SLOT(quitMenu()));

}

void MainWindow::hardquit()
{
    player->despawn(); // completely closes the program
    m_socket->disconnectFromHost();
    emit stop();
    connect(this, SIGNAL(stop()), this, SLOT(close()));
}

void MainWindow::quittoMain() // returns the player to the main menu of the game (page 0)
{
    ui->stackedWidget->setCurrentIndex(quitConfirm);
    currentind = ui->stackedWidget->currentIndex();
    connect(ui->yesButton, SIGNAL(clicked()), this, SLOT(mainMenu()));
    connect(ui->noButton, SIGNAL(clicked()), this, SLOT(quitMenu()));

}

void MainWindow::helpMenu() // displays a help page which describes game controls and objective.
{
    currentind = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(help);
    connect(ui->returnButton_2, SIGNAL(clicked()), this, SLOT(returntoMenu()));

}

void MainWindow::mainMenu() //despawning of player and client disconnection from server should be here
{
    if (currentind == quitConfirm)
    {
        player->despawn();
        m_socket->disconnectFromHost();
    }
    ui->stackedWidget->setCurrentIndex(start);
}

void MainWindow::returntoMenu()
{
    ui->stackedWidget->setCurrentIndex(currentind);
}

void MainWindow::returntogame()
{
    ui->stackedWidget->setCurrentIndex(gameplay);
}

void MainWindow::loadingScreen()
{
    QString ip;
    QString port;
    if(ui->ipEdit->text() != NULL) // if IP and port have been provided, call joinserver with these parameters.
    {
        ip = ui->ipEdit->text();
        qDebug() << "Ip:" << ip;
    }
    if(ui->portEdit->text() != NULL)
    {
        port = ui->portEdit->text();
        qDebug() << "port:" << port;
    }

    ui->stackedWidget->setCurrentIndex(2);
    joinServer(ip, port.toInt(), m_socket);

}

void MainWindow::joinGame()
{
    qDebug() << m_socket->state();
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::spawnPlayer()
{
    // Spawn custom Player object in center of scene
    player = new Player;
    player->setPos(0,8);
    scene->addItem(player);
}

void MainWindow::spawnFish(int x, int y)
{
        fish = new Fish;
        fish->setPos(x, y); // set position randomly inside here to ensure coordinates don't get confused during fish.cpp
        scene->addItem(fish); // fish.cpp is picking the random coords and painting *ONE FISH* per cal of scene->addItem(fish). If you want one fish, call it outside this loop somewhere.
         // if the fish emits a hit, then that means someone collided and left clicked it, so we go to fishCollision below...
        connect(fish, &Fish::hit, this, &MainWindow::fishCollision);
}

void MainWindow::fishCollision(int hitX, int hitY)
{
        //qDebug() << "Fish has been caught at " << hitX << "," << hitY;
        cn = QString::number(client_num); // cn specifies the order that each client connected (player 1,2,3,4).
        qint32 x = QRandomGenerator::global()->bounded(-SCENE_WIDTH/2 + 10,SCENE_WIDTH/2 - 10);
        qint32 y = QRandomGenerator::global()->bounded(-SCENE_HEIGHT/2 + 10,SCENE_HEIGHT/2 - 10);
        spawnFish(x,y);
        player1score++;
        ui->score->setNum(player1score);

// * since full network implementation did not work out, each player will see their score in the player1 slot (red bucket), but once the game has ended they will see their score in the proper spot
}

void MainWindow::UpdateTimer()
{
    timercountdown --;
    // count down the timer each second until 10 seconds is left on the clock. this will move to the game over/final scores

    if(timercountdown == 0)
    {   qDebug() << "Game Over Time is up!";
        label->setText("");    // get rid of the counter
        cn = QString::number(client_num);
        QString score = QString::number(player1score);
        QString data = "go:" +cn + ":" +score +"\n";
        QByteArray sendData = data.toUtf8();
        m_socket->write(sendData);
    }
    else if (timercountdown < 0) {
        // do nothing, just please stop printing the debug message every second!
    }
    else
    {
        label->setText(QString::number(timercountdown));
    }
}

void MainWindow::joinServer(QString ip, int port, QTcpSocket*m_socket)
{
    //Connects user to server
     if (m_socket->state() != QAbstractSocket::ConnectedState) {
         qDebug() << m_socket->state();
         m_socket->connectToHost(ip, port); // connect to the host based on Adr and Port from join menu text
         if (!m_socket->waitForConnected(3000)) // Wait 3 seconds
         {
             ui->stackedWidget->setCurrentIndex(0);
         }
    }
        qDebug() << m_socket->state();
        connect(m_socket, &QTcpSocket::connected,
                    this, &MainWindow::joinGame);
        qDebug() << m_socket->state();
         ui->stackedWidget->setCurrentIndex(loading);
         connect(m_socket, &QTcpSocket::readyRead,
                 this, &MainWindow::readyRead);
}

void MainWindow::readyRead()
{
    //qDebug() << "Entered readyRead function";
    QByteArray data = m_socket->readLine();
    QString data_string =QString::fromStdString(data.toStdString());
    data_string = data_string.simplified();
    if (data_string == "START")
    {
        emit ready(); // ready signal will start the game once START has been called from the server
        ui->stackedWidget->setCurrentIndex(gameplay);
    }
    int size = data_string.length();
    if (data_string.left(3) == "x: ") // if the current data matches particular formats, we want to use it in different ways
    {
        fishX = (data_string.mid(4, size)).toInt();
        fishX += 50;
    }
    if (data_string.left(3) == "y: ") // positioning data
    {
        fishY = data_string.mid(4, size).toInt();
        fishY += 50;
        spawnFish(fishX, fishY);
    }

    if (data_string.left(4) == "pn: ") // player number data
    {
        client_num = data_string.mid(4).toInt();
        emit(sendPlayer(client_num));
    }

    if (data_string.left(3) == "hx:") // hit reg data
    {
        QString x_str = data_string.mid(3,1);
        x_hit = x_str.toInt();
        //qDebug() << "hx:" +x_str;
    }

    if (data_string.left(3) == "hy:")
    {
        QString y_str = data_string.mid(3,1);
        y_hit = y_str.toInt();
        //qDebug() << "hy:" +y_str;
    }

    if (data_string.left(4) == "sb1:") // scoreboard final values
    {
        player1scoreb = data_string.mid(4).toInt();
        qDebug() << "p1s: " << player1scoreb;
    }
    if (data_string.left(4) == "sb2:")
    {
        player2scoreb = data_string.mid(4).toInt();
        qDebug() << "p2s: " << player2scoreb;
    }
    if (data_string.left(4) == "sb3:")
    {
        player3scoreb = data_string.mid(4).toInt();
        qDebug() << "p3s: " << player3scoreb;
    }
    if (data_string.left(4) == "sb4:")
    {
        player4scoreb = data_string.mid(4).toInt();
        qDebug() << "p4s: " << player4scoreb;
        UpdateScoreBoard(); // once the last scoreboard value is received, show the scores
    }
}

void MainWindow::InGameTime()
{
    // Timer for Game Clock
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(UpdateTimer())); // update the
    timer->start(1000); // 1000 milliseconds -> 1 second // two minutes is going

    label = new QLabel(this);
    label->setText("Start"); // ignore this. I was not having fun trying to get this working
    // HOW ON EARTH DO I MOVE THIS THING TO THE RIGHT MORE????
    label->setGeometry(650, 20, 131, 41);
    label->setAlignment(Qt::AlignCenter);
    label->show();
}

void MainWindow::UpdateScoreBoard()
{
    ui->p1score->setNum(player1scoreb);
    ui->p2score->setNum(player2scoreb);
    ui->p3score->setNum(player3scoreb);
    ui->p4score->setNum(player4scoreb);
    ui->stackedWidget->setCurrentIndex(4); // Command to send the game to the next page
}

void MainWindow::startGame()
{
    InGameTime(); // set up timer and player
    spawnPlayer();

    QTimer* timer3 = new QTimer;
    timer3->setInterval(FRAME_RATE); // refresh rate for the gameplay

    connect(timer3, &QTimer::timeout, scene, &QGraphicsScene::advance); // whenever our timer3 times out (refresh rate), update the scene and network data
    connect(timer3, &QTimer::timeout, this, &MainWindow::readyRead);
    timer3->start();
}
