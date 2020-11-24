#ifndef QGLGEARS_H
#define QGLGEARS_H

#ifndef QT_NO_OPENGL
#include <QGLWidget>


#include "commonrenderer.h"

class QPaintEvent;
class QPainter;

class QGLGears : public QGLWidget,
                 public CommonRenderer
{
    Q_OBJECT
public:
    QGLGears();

protected:
    virtual void paintEvent(QPaintEvent *e);
};

#endif
#endif
