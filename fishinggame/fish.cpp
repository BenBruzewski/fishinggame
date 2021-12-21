/*
 * Fishing Simulator 2021
 * CPE 353, Fall 2021
 * Made by:
 * Ben Bruzewski, Ben McAnulty, Jacob Smith, and Jesse Cowart
*/

#include "fish.h"
#include "mainwindow.h"
#include "defs.h"
#include <QRandomGenerator64>

Fish::Fish(QGraphicsItem *parent) : QGraphicsObject (parent)
{

    QString f = ":/images/Fish.png";
    QString g = ":/images/Flounder.png";
    QString h = ":/images/Pufferfish.png";
    fishpic[0] = QPixmap(f);
    fishpic[1] = QPixmap(g);
    fishpic[2] = QPixmap(h);
    fishpic[0] = fishpic[0].scaled(FISH_WIDTH, FISH_HEIGHT);
    fishpic[1] = fishpic[1].scaled(FISH_WIDTH, FISH_HEIGHT);
    fishpic[2] = fishpic[2].scaled(FISH_WIDTH, FISH_HEIGHT); // generating an array of 3 pixmaps for our 3 fish images.
}

Fish::~Fish()
{

}

QRectF Fish::boundingRect() const
{
    return QRectF(-FISH_WIDTH/2, -FISH_HEIGHT/2, FISH_WIDTH, FISH_HEIGHT);
}

QPainterPath Fish::shape() const
{
    QPainterPath path;
    path.addRect(-FISH_WIDTH/2, -FISH_HEIGHT/2, FISH_WIDTH, FISH_HEIGHT);
    return path;
}

void Fish::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // all the paint() function does now is pick an image and draw the pixmap, mainwindow.cpp handles the rest
        //qDebug() << "Entered fish.cpp's Paint function";
        qint32 pickFish = QRandomGenerator::global()->bounded(0,3); // made a rand num generator for the fish to choose an image
        painter->drawPixmap(-FISH_WIDTH/2, -FISH_HEIGHT/2,FISH_WIDTH, FISH_HEIGHT, fishpic[pickFish]); // the mainwindow.cpp function will be using setPos to move each fish as it is called, so just place them at 0,0 for now...
        // for any curious readers, this drawPixmap() function was the bane of my existence and was why fish weren't fully deleting upon collison - Ben B.
}

void Fish::advance(int phase)
{
    // Fish does not move
    if (phase == 0)
    {
        return;
    }
    QList<QGraphicsItem*> list = collidingItems();
    if (!list.isEmpty())
    {
        qDebug() << collidingItems();
        QGraphicsItem* hitter = list[0];
        emit hit(hitter->x(), hitter->y()); // report the "caught" fish's coordinates to hit().
        this->scene()->removeItem(this); // remove fish once collision is detected
        this->deleteLater();
    }
}
