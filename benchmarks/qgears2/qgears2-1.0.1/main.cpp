#include "qgears.h"
#include "qglgears.h"

#include <QApplication>
#include <QtDebug>

#if defined(Q_WS_X11)
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/extensions/Xrender.h>
#endif

enum RenderType
{
    Render,
    OpenGL,
    Image
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    RenderType renderer = Render;
    CommonRenderer::Mode mode = CommonRenderer::GEARSFANCY;
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp("-gl", argv[i])) {
            renderer = OpenGL;
        }
        else if (!strcmp("-render", argv[i])) {
            renderer = Render;
        }
        else if (!strcmp("-image", argv[i])) {
            renderer = Image;
        } else if (!strcmp("GEARSFANCY", argv[i])) {
            mode = CommonRenderer::GEARSFANCY;
        } else if (!strcmp("GEARS", argv[i])) {
            mode = CommonRenderer::GEARS;
        } else if (!strcmp("COMPO", argv[i])) {
            mode = CommonRenderer::COMPO;
        } else if (!strcmp("TEXT", argv[i])) {
            mode = CommonRenderer::TEXT;
        } else if (!strcmp("RAWPIXMAP", argv[i])) {
            mode = CommonRenderer::RAWPIXMAP;
        }
        else {
            if (i == (argc-1)) {
                fprintf(stderr, "Usage: %s [-gl -render -image] [GEARSFANCY GEARS COMPO TEXT]\n", argv[0]);
                exit(1);
            }
        }
    }

    QWidget *widget = 0;

    switch (renderer) {
    case OpenGL:
#ifndef QT_NO_OPENGL
        widget = new QGLGears();
#else
        qWarning("OpenGL not supported!");
        exit(1);
#endif
        break;
    case Render:
        widget = new QGears(false);
        break;
    case Image:
        widget = new QGears(true);
        break;
    }
    CommonRenderer *rendererWidget = dynamic_cast<CommonRenderer*>(widget);
    if (rendererWidget) {
        //qDebug()<<"setting mode to "<<mode;
        rendererWidget->setMode(mode);
    }

    widget->show();
    app.exec();
}
