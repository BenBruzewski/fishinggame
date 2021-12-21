/*
 * Fishing Simulator 2021
 * CPE 353, Fall 2021
 * Made by:
 * Ben Bruzewski, Ben McAnulty, Jacob Smith, and Jesse Cowart
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QPainter>
#include <QtDebug>
#include <QRandomGenerator>
#include "defs.h"

class Player : public QGraphicsItem
{
public:
    Player(QGraphicsItem *parent = nullptr);
    ~Player() override;

    // Making the player model/hitbox
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  //  void advance(int phase) override;

    void goLeft();
    void goRight();
    void goUp();
    void goDown();
    void stop();
    void despawn();

private:

    QPixmap image;
    int index;
    int offset;
    QPixmap images[4];
    void advance(int phase) override;

    qreal x = 0.0;                 // Current player position (x,y)
    qreal y = 0.0;

    qreal player_dx = 5.0;         // Fixed increments of travel
    qreal player_dy = 5.0;

    qreal dx = 0.0;                // Desired direction of travel
    qreal dy = 0.0;

    int pickPlayer = QRandomGenerator::global()->bounded(1,5);

    // randomly assign each player a character model.

private slots:
        void receivePlayer(int player_num);
};

#endif // PLAYER_H
