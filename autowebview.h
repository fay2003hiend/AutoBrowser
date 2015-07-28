#ifndef AUTOWEBVIEW_H
#define AUTOWEBVIEW_H

#include <QWebPage>
#include <QWebView>
#include <QNetworkReply>
#include <QSslError>
#include <QNetworkCookieJar>
#include <QNetworkDiskCache>

static QNetworkCookieJar *m_cookies;
static QNetworkDiskCache *m_cache;

class AutoWebPage : public QWebPage {
public:
    AutoWebPage(QObject * parent = 0);
    QString	userAgentForUrl ( const QUrl & url ) const;
};

class AutoWebView : public QWebView {
    Q_OBJECT
public:
    explicit AutoWebView(QWidget *parent = 0);
    QWebView* createWindow(QWebPage::WebWindowType);
public slots:
    void slot_clearCache();
private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
};

#endif // AUTOWEBVIEW_H
