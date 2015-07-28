#include "autowebview.h"
#include "widgetmain.h"

#include <QApplication>

AutoWebPage::AutoWebPage(QObject *parent)
    : QWebPage(parent) {

}

QString AutoWebPage::userAgentForUrl(const QUrl &url) const {
    return QWebPage::userAgentForUrl(url);
}

AutoWebView::AutoWebView(QWidget *parent) :
    QWebView(parent) {
    if(m_cookies == NULL) {
        m_cookies = new QNetworkCookieJar(qApp);
    }
    if(m_cache == NULL) {
        m_cache = new QNetworkDiskCache(qApp);
        m_cache->setCacheDirectory("_cache");
        m_cache->setMaximumCacheSize(512*1024*1024);
    }
    DBG("m_cookies = %p, m_cache = %p\n", m_cookies, m_cache);

    setPage(new AutoWebPage(this));
    QNetworkAccessManager * pNAM = page()->networkAccessManager();

    pNAM->setCookieJar(m_cookies);
    pNAM->setCache(m_cache);

    DBG("after set cookies and cache\n");
    m_cookies->setParent(qApp);
    m_cache->setParent(qApp);

    //qreal zoom = HdpiTool::getZoomFactor(qApp);
    //setZoomFactor(zoom);

    connect(pNAM, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));
}

QWebView *AutoWebView::createWindow(QWebPage::WebWindowType) {
    WidgetMain *w = new WidgetMain();
    w->show();
    return w->getWebView();
}

void AutoWebView::slot_clearCache() {
    if(m_cache != NULL) {
        m_cache->clear();
    }
}

void AutoWebView::handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors) {
    foreach(QSslError err, errors) {
        DBG("%p SSLError: %s\n", this, err.errorString().toLocal8Bit().data());
    }
    reply->ignoreSslErrors();
}
