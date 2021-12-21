/*
 * Fishing Simulator 2021
 * CPE 353, Fall 2021
 * Made by:
 * Ben Bruzewski, Ben McAnulty, Jacob Smith, and Jesse Cowart
*/

#ifndef FISH_H
#define FISH_H

#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <QtDebug>
#include <QTcpSocket>
#include "defs.h"

class Fish : public QGraphicsObject // inherits from QGobject which lets us use connect
{
    Q_OBJECT

public:
    Fish(QGraphicsItem *parent = nullptr);
    ~Fish() override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void advance(int phase) override;

signals:
    void hit(int x, int y);         // Signal emitted upon collision between Fish and ANY object

private:
        QPixmap fishpic[3]; // array for our 3 fish assets

private slots:
};

#endif // Fish_H
