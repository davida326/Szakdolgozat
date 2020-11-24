#ifndef RAWPIXMAP_H
#define RAWPIXMAP_H

#include <QPixmap>

class QPainter;

class RawPixmap
{
public:
    RawPixmap();
    void render(QPainter *p, int w, int h);

private:
    QPixmap pix;
};

#endif
