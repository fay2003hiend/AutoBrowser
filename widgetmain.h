#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QWidget>
#include <QUrl>

#define DBG(fmt, ...) printf("%s %d: "fmt, __FILE__, __LINE__, __VA_ARGS__)

namespace Ui {
class WidgetMain;
}

class QWebView;

class WidgetMain : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetMain(QWidget *parent = 0);
    ~WidgetMain();

    QWebView* getWebView();
    void loadUrl(const QUrl& url);
private slots:
    void slot_loadFromAddressBar();
    void slot_urlChanged(const QUrl& url);
    void slot_titleChanged(const QString& title);
    void slot_loadStarted();
    void slot_loadFinished();

    void slot_checkForAutomate();
    void slot_automateNow();
    void slot_addLog(const QString &log);
private:
    Ui::WidgetMain *ui;

    bool isLoading;
    uint automate_time;
};

#endif // WIDGETMAIN_H
