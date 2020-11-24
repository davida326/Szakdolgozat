#ifndef FDCLOCK_H
#define FDCLOCK_H

class QPainter;

class FdClock
{
public:
    static void faceDraw(QPainter *p, double width,  double height);
    static void logoDraw(QPainter *p, double width, double height);
    static void handDraw(QPainter *p, double width, double height,
                         int seconds);

    static void render(QPainter *p, double width, double height);
};

#endif
