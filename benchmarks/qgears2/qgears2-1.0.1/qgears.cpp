#include "qgears.h"

#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QPaintEngine>

QGears::QGears(bool imageBased)
    : QWidget(),
      m_imageBased(imageBased)
{
    //suprisingly flickers very badly
    //if (m_imageBased)
    //setAttribute(Qt::WA_PaintOnScreen);
    //setAttribute(Qt::WA_NoBackground);
    //setAttribute(Qt::WA_NoSystemBackground);
    setFixedSize(512, 512);
}

void QGears::paintEvent(QPaintEvent *)
{
    if (m_imageBased) {
        QImage image(size(), QImage::Format_ARGB32_Premultiplied);
        renderTo(&image);
        QPainter painter(this);
        if (painter.paintEngine()->hasFeature(QPaintEngine::PorterDuff))
            painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(QPoint(0, 0), image);
    } else {
        renderTo(this);
    }

    QTimer::singleShot(0, this, SLOT(repaint()));
}
