#include "text.h"

#include <QPainter>

#define N_GLYPHS 2*2142

Text::Text()
    : pen(QColor(0, 0, 0)),
      font("Sans", 10, QFont::Normal)
{
    QString base("abcdefghijklmnoprstuvwxyz");
    font.setPixelSize(10);

    QFontMetrics metrics(font);

    int baseWidth = metrics.width(base);
    int currentLine = 0;
    for (int i = 0; i < N_GLYPHS; ++i) {
        str += base;
        i += base.length();
        currentLine += baseWidth;
        if (currentLine >= 512) {
            currentLine = 0;
            str += "\n";
        }
    }
}


void Text::render(QPainter *p, int w, int h)
{
    QRectF rect(0, 0, w, h);
    p->setClipRect(rect);
    p->setPen(pen);
    p->setFont(font);
    p->drawText(rect, Qt::TextWordWrap, str);
}
