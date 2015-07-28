#include "widgetmain.h"
#include "ui_widgetmain.h"

#include <QWebElement>
#include <QWebPage>
#include <QWebFrame>

#include <QTimer>
#include <QDateTime>

WidgetMain::WidgetMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMain),
    isLoading(false),
    automate_time(0)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(returnPressed()), SLOT(slot_loadFromAddressBar()));

    connect(ui->webView, SIGNAL(urlChanged(QUrl)), SLOT(slot_urlChanged(QUrl)));
    connect(ui->webView, SIGNAL(titleChanged(QString)), SLOT(slot_titleChanged(QString)));
    connect(ui->webView, SIGNAL(loadStarted()), SLOT(slot_loadStarted()));
    connect(ui->webView, SIGNAL(loadFinished(bool)), SLOT(slot_loadFinished()));

    QTimer *tm = new QTimer(this);
    tm->setInterval(500);
    connect(tm, SIGNAL(timeout()), SLOT(slot_checkForAutomate()));
    tm->start();
}

WidgetMain::~WidgetMain()
{
    delete ui;
}

QWebView *WidgetMain::getWebView() {
    return ui->webView;
}

void WidgetMain::loadUrl(const QUrl &url) {
    ui->webView->load(QUrl(url));
}

QString getLinkTitle(QWebElement &element) {
    QString title = element.toPlainText().simplified();
    if(!title.isEmpty()) {
        return title;
    }

    QWebElement img = element.findFirst("img");
    if(!img.isNull()) {
        title = img.attribute("alt");
    } else if(element.hasAttribute("title")) {
        title = element.attribute("title");
    } else if(element.hasAttribute("alt")) {
        title = element.attribute("alt");
    }
    return title;
}

QString getTitleFromLeft(QWebElement &element) {
    QString title;
    QWebElement el = element;
    while(!el.isNull()) {
        QWebElement prev = el.previousSibling();
        while(!prev.isNull()) {
            if(prev.tagName() != "A") {
                title = prev.toPlainText().simplified();
                if(!title.isEmpty()) {
                    break;
                }
            }
            prev = prev.previousSibling();
        }
        if(!title.isEmpty()) {
            break;
        }
        el = el.parent();
    }
    //DBG("getTitleFromLeft '%s'\n", title.toLocal8Bit().data());
    return title;
}

QString getTitleFromRight(QWebElement &element) {
    QString title;
    QWebElement el = element;
    while(!el.isNull()) {
        QWebElement next = el.nextSibling();
        while(!next.isNull()) {
            if(next.tagName() != "A") {
                title = next.toPlainText().simplified();
                if(!title.isEmpty()) {
                    break;
                }
            }
            next = next.nextSibling();
        }

        if(!title.isEmpty()) {
            break;
        }
        el = el.parent();
    }
    //DBG("getTitleFromRight '%s'\n", title.toLocal8Bit().data());
    return title;
}

QString getTitle(QWebElement &element) {
    QString title;
    do {
        if(!(title = element.attribute("title")).isEmpty()) {
            break;
        }
        if(!(title = element.attribute("alt")).isEmpty()) {
            break;
        }
        if(!(title = element.attribute("placeholder")).isEmpty()) {
            break;
        }
        title = getTitleFromLeft(element);
    } while(0);
    return title;
}

QString getRadioTitle(QWebElement &element) {
    QWebElement el = element.parent();
    QString value = getTitleFromRight(element);
    while(el.toPlainText().simplified() == value) {
        el = el.parent();
    }
    return getTitleFromLeft(el);
}

void traverseHtml(QList<QWebElement>& list, QWebElement& root) {
    list.append(root);
    QWebElement el = root.firstChild();
    while(!el.isNull()) {
        traverseHtml(list, el);
        el = el.nextSibling();
    }
}

void printNodeAttr(QWebElement &node) {
#ifdef CMD_DEBUG
    QStringList attrList = node.attributeNames();
    printf("Attribute names ==> ");
    foreach(const QString& attrName, attrList) {
        if(attrName.startsWith("on")) {
            printf("[%s] ", attrName.toLocal8Bit().data());
        }
    }
    printf("\n");
#endif
}

void WidgetMain::slot_loadFromAddressBar() {
    QString url = ui->lineEdit->text();
    if(!url.startsWith("https://")
            && !url.startsWith("http://")
            && !url.startsWith("file:///")) {
        url.insert(0, "http://");
        ui->lineEdit->setText(url);
    }
    ui->webView->load(QUrl(url));
}

void WidgetMain::slot_urlChanged(const QUrl &url) {
    ui->lineEdit->setText(url.toString());
}

void WidgetMain::slot_titleChanged(const QString &title) {
    setWindowTitle(title);
}

void WidgetMain::slot_loadStarted() {
    isLoading = true;
}

void WidgetMain::slot_loadFinished() {
    isLoading = false;
    automate_time = QDateTime::currentDateTime().toTime_t() + 2; // 2 seconds after loaded
}

void WidgetMain::slot_checkForAutomate() {
    if(automate_time > 0 && !isLoading) {
        if(QDateTime::currentDateTime().toTime_t() >= automate_time) {
            slot_automateNow();
            automate_time = 0;
        }
    }
}

void WidgetMain::slot_automateNow() {
    QString currentUrl = ui->webView->url().toString();
    QWebElement root = ui->webView->page()->mainFrame()->documentElement();
    slot_addLog(tr("start automation <font color='blue'><b>[%1]</b></font>").arg(currentUrl));

    QList<QWebElement> nodeList;
    QWebElement bodyElement = root.findFirst("body");
    traverseHtml(nodeList, bodyElement);

    QString tag, type, title;
    foreach(QWebElement node, nodeList) {
        tag = node.tagName();
        if(tag == "INPUT") {
            type = node.attribute("type");
            if(type == "hidden") {
                continue;
            } else if(type == "button" || type == "submit") {
                title = node.attribute("value");
            } else if(type == "radio") {
                title = getRadioTitle(node);
            } else if(type == "checkbox" ){
                QString Lt = getTitleFromLeft(node);
                QString Rt = getTitleFromRight(node);
                if(Rt.length() > Lt.length()) {
                    title = Rt;
                } else {
                    title = Lt;
                }
            } else {
                title = getTitle(node);
            }

            slot_addLog(QString("input type=[%1] title=[%2]").arg(type, title));
            /* demo demo demo */
            if(type == "email") {
                slot_addLog(tr("<font color='red'>## fill in email addr</font>"));
                node.setAttribute("value", "fay2003hiend@gmail.com");
            } else if(type == "submit") {
                slot_addLog(tr("<font color='red'>## click submit</font>"));
                node.evaluateJavaScript("this.click()");
            }
        } else if(tag == "SELECT") {
            title = getTitle(node);

            slot_addLog(QString("select title=[%1]").arg(title));
            /* demo demo demo */
            QWebElementCollection options = node.findAll("option");
            if(options.count() > 1) {
                slot_addLog(tr("<font color='red'>## select 2nd option</font>"));
                node.evaluateJavaScript(QString("this.selectedIndex = 1"));
                if(node.hasAttribute("onchange")) {
                    node.evaluateJavaScript(node.attribute("onchange"));
                }
            }
        } else if(tag == "A") {
            title = getLinkTitle(node);

            slot_addLog(QString("link title=[%1]").arg(title));
            /* demo demo demo */
        } else if(tag == "BUTTON") {
            title = getTitle(node);

            slot_addLog(QString("button title=[%1]").arg(title));
            /* demo demo demo */
        }
    }
}

void WidgetMain::slot_addLog(const QString &log) {
    QString now = QDateTime::currentDateTime().toString("[hh:mm:ss:zzz] ");
    ui->textBrowser->append(now + log);
}
