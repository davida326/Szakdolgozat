#include "qglgears.h"
#ifndef QT_NO_OPENGL
#include <QTimer>

QGLGears::QGLGears()
    : QGLWidget(QGLFormat(QGL::SampleBuffers), 0)
{
    setFixedSize(512, 512);
}

void QGLGears::paintEvent(QPaintEvent *)
{

    renderTo(this);
    QTimer::singleShot(0, this, SLOT(repaint()));
}
#endif
