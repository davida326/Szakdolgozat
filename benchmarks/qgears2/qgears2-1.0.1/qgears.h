#ifndef QGEARS_H
#define QGEARS_H

#include <QWidget>

#include "commonrenderer.h"

class QPaintEvent;
class QPainter;

class QGears : public QWidget,
               public CommonRenderer
{
    Q_OBJECT
public:
    QGears(bool imageBased);

protected:
    virtual void paintEvent(QPaintEvent *e);
private:
    bool m_imageBased;
};

#endif
