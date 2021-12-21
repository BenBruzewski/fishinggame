/*
 * Fishing Simulator 2021
 * CPE 353, Fall 2021
 * Made by:
 * Ben Bruzewski, Ben McAnulty, Jacob Smith, and Jesse Cowart
*/

#include "player.h"
#include "mainwindow.h"
#include <QDialog>


Player::Player(QGraphicsItem *parent)  : QGraphicsItem(parent)
{

    QString s = ":/images/player1.png";
    QString t = ":/images/player2.png";
    QString u = ":/images/player3.png";
    QString v = ":/images/player4.png";

    images[0] = QPixmap(s);
    images[1] = QPixmap(t);
    images[2] = QPixmap(u);
    images[3] = QPixmap(v);

    images[0] = images[0].scaled(PLAYER_WIDTH, PLAYER_HEIGHT);
    images[1] = images[1].scaled(PLAYER_WIDTH, PLAYER_HEIGHT);
    images[2] = images[2].scaled(PLAYER_WIDTH, PLAYER_HEIGHT);
    images[3] = images[3].scaled(PLAYER_WIDTH, PLAYER_HEIGHT);

    index = 0;
    offset = 18;

}

Player::~Player()
{

}

void Player::goLeft()
{
    dx = -player_dx; // by default, dx and dy are set to 5 pixels.
    dy = 0.0;
}

void Player::goRight()
{
    dx = player_dx;
    dy = 0.0;
}

void Player::goUp()
{
    dx = 0.0;
    dy = -player_dy;
}

void Player::goDown()
{
    dx = 0.0;
    dy = player_dy;;
}

void Player::stop()
{
    dx = 0.0;
    dy = 0.0;
}

QRectF Player::boundingRect() const
{
    return QRectF(-PLAYER_WIDTH/2, -PLAYER_HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT);
}

QPainterPath Player::shape() const
{
    QPainterPath path;
    path.addRect(-PLAYER_WIDTH/2, -PLAYER_HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT);
    return path;
}

void Player::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    painter->drawPixmap(-PLAYER_WIDTH/2, -PLAYER_HEIGHT/2, images[pickPlayer-1]); // to draw the other player models, replace images[0] with images[1,2, or 3].
    // unfortunately could not send a signal/slot into the player to specify order of connections, so each player will receive a random model instead.
}

void Player::advance(int phase)
{
    if (phase == 0)
    {
        return;
    }

    // Compute new coordinates
    x = this->pos().rx();
    y = this->pos().ry();
    x = x + dx;
    y = y + dy;

    // Force new coordinates to remain within scene bounds
    if (x < -SCENE_WIDTH/2 + PLAYER_WIDTH/2)
    {
        x = -SCENE_WIDTH/2 + PLAYER_WIDTH/2;
    }
    else if (x > SCENE_WIDTH/2 - PLAYER_WIDTH/2)
    {
        x = SCENE_WIDTH/2 - PLAYER_WIDTH/2;
    }

    if (y < -SCENE_HEIGHT/2 + PLAYER_HEIGHT/2)
    {
        y = -SCENE_HEIGHT/2 + PLAYER_HEIGHT/2;
    }
    else if (y > SCENE_HEIGHT/2 - PLAYER_HEIGHT/2)
    {
        y = SCENE_HEIGHT/2 - PLAYER_HEIGHT/2;
    }

    // Move player to new coordinates
    this->setPos(x,y);

    // Detect any collisions
    QList<QGraphicsItem*> list = collidingItems();
    if (!list.isEmpty())
    {
        //qDebug() << "Player detects collision";
    }
}

void Player::despawn()
{
    this->scene()->removeItem(this);
    qDebug() << "Player has been despawned!";
    // this->deleteLater();
}
