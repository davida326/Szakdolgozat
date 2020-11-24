#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <QPixmap>

class QPainter;

class Composite
{
public:
    Composite();
    void render(QPainter *p, int w, int h);

private:
    QPixmap bg;
    QPixmap fg;

    qreal comp_x, comp_y;
    qreal comp_x_dir, comp_y_dir;

    qreal oversize;
    qreal oversize_dir;
};

#endif
