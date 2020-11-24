#include "composite.h"

#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QPaintEngine>

extern double drand();

Composite::Composite()
    : bg("bg.png"),
      fg("fg.png")
{
    comp_x = 400 - 256 / 2;
    comp_y = 200 - 256 / 2;
    comp_x_dir = 5.0;
    comp_y_dir = 5.0;

    oversize = 1.0;
    oversize_dir = 0.001;
}

void Composite::render(QPainter *p, int w, int h)
{
    double scale_x, scale_y;

    p->save();

    scale_x = ((double) w) / (double) bg.width();
    scale_y = ((double) h) / (double) bg.height();

    //p->translate(-((oversize - 1.0) * (double) w) / 2.0,
    //           -((oversize - 1.0) * (double) h) / 2.0);
    p->scale(scale_x * oversize, scale_y * oversize);
    if (p->paintEngine()->hasFeature(QPaintEngine::PorterDuff))
        p->setCompositionMode(QPainter::CompositionMode_Source);
    QPointF pt(-((oversize - 1.0) * (double) w) / 2.0,
               -((oversize - 1.0) * (double) h) / 2.0);
    p->drawPixmap(pt,
                  bg);
    p->restore();

    oversize += oversize_dir;
    if (oversize >= 1.2)
        oversize_dir = -oversize_dir;
    if (oversize <= 1.0)
        oversize_dir = -oversize_dir;

    p->save();
    if (p->paintEngine()->hasFeature(QPaintEngine::PorterDuff))
        p->setCompositionMode(QPainter::CompositionMode_SourceOver);

    if (comp_x_dir < 0) {
        if (comp_x_dir < -1.0)
            comp_x_dir += 10.0 / w;
    } else {
        if (comp_x_dir > 1.0)
            comp_x_dir -= 10.0 / w;
    }

    if (comp_y_dir < 0) {
        if (comp_y_dir < -1.0)
            comp_y_dir += 10.0 / h;
    } else {
        if (comp_y_dir > 1.0)
            comp_y_dir -= 10.0 / h;
    }

    comp_x += comp_x_dir;
    comp_y += comp_y_dir;

    if (comp_x >= (w - fg.width()))
        comp_x_dir = -(drand() * 5.0 + 1.0 + comp_x_dir / 2.0);
    else if (comp_x <= 0)
        comp_x_dir = (drand() * 5.0 + 1.0 + comp_x_dir / 2.0);

    if (comp_y >= (h - fg.height()))
        comp_y_dir = -(drand() * 5.0 + 1.0 + comp_x_dir / 2.0);
    else if (comp_y <= 0)
        comp_y_dir = drand() * 5.0 + 1.0 + comp_x_dir / 2.0;

    pt = QPointF(comp_x, comp_y);
    p->drawPixmap(pt, fg);
    p->restore();
}

