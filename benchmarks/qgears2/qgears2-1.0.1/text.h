#ifndef TEXT_C
#define TEXT_C

#include <QPen>
#include <QFont>
#include <QString>
class QPainter;

class Text
{
public:
    Text();
    void render(QPainter *p, int w, int h);
private:
    QPen pen;
    QFont font;
    QString str;
};

#endif
