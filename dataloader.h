#ifndef DATALOADER_H
#define DATALOADER_H

#include <QObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QMainWindow>
#include <QTextCodec>
#include <fstream>

class DataLoader : public QObject
{
    Q_OBJECT

public:
    explicit DataLoader(QMainWindow* pMainWindow);

    void get(int maxResults, const QString& url);
    void clearURLs();
    const std::vector<QString>& resourceURLs();
    void fillSentences();

public slots:
    void queryPerformed(QNetworkReply* pReply);
    void contentPerformed(QNetworkReply* pReply);

private:
    void parseResponse(const QString& response);
    void parseContent(const QString& content);

private:
    QNetworkAccessManager* m_pManager = nullptr;
    std::vector<QString> m_aResourceURLs;
    QMainWindow* m_pMainWindow;
    int m_maxResults, m_curResult;
    std::ofstream m_out;
};

#endif // DATALOADER_H
