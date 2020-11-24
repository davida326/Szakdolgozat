#include "rawpixmap.h"

#include <QPainter>

RawPixmap::RawPixmap()
    : pix("bg.png")
{
}

void RawPixmap::render(QPainter *p, int w, int h)
{
    p->setCompositionMode(QPainter::CompositionMode_Source);
    p->drawPixmap(0, 0, pix);
}
