#include "widgetmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetMain w;
    w.show();

    w.loadUrl(QUrl("https://gmail.com"));

    return a.exec();
}
