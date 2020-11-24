#ifndef COMMONRENDERER_H
#define COMMONRENDERER_H

class QPainter;
class QWidget;
class QImage;
class QPixmap;

#include <QTime>
#include <QPainterPath>

#include "composite.h"
#include "text.h"
#include "rawpixmap.h"

#define NUMPTS 6

class CommonRenderer
{
public:
    enum Mode
    {
        GEARSFANCY,
        GEARS,
        COMPO,
        TEXT,
        RAWPIXMAP
    };
public:
    CommonRenderer();
    virtual ~CommonRenderer();

    void setMode(Mode mode);
protected:
    virtual void renderTo(QPaintDevice *device);

    void gears(QPaintDevice *device);

    QPainterPath gearPath(double inner_radius, double outer_radius,
                          int teeth, double tooth_depth) const;
    void pathRender(QPainter *p, QPaintDevice *device);
    void gearsRender(QPainter *p);

    void setup(int w, int h);
    void animate(double *pts, double *deltas,
                 int index, int limit);
    void animateStep(int w, int h);
    void printFrameRate();

    QPainterPath m_gear1;
    QPainterPath m_gear2;
    QPainterPath m_gear3;

    bool setupFinished;

    qreal animpts[NUMPTS * 2];
    qreal deltas[NUMPTS * 2];

    Composite compo;
    Text text;
    RawPixmap rawpix;
    Mode mode;

    QTime ptime;
    unsigned int frame_cnt;
};

#endif
