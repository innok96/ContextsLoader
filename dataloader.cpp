#include "dataloader.h"
#include <iostream>
#include <regex>
#include <QMessageBox>
#include <QTextDocument>

DataLoader::DataLoader(QMainWindow* pMainWindow) :
    QObject(nullptr),
    m_pMainWindow(pMainWindow)
{
    m_pManager = new QNetworkAccessManager(this);
}

void DataLoader::queryPerformed(QNetworkReply* pReply)
{
    if (pReply->error() == QNetworkReply::NoError)
    {
        QByteArray content= pReply->readAll();
        QTextCodec* pCodec = QTextCodec::codecForName("utf8");

        parseResponse(pCodec->toUnicode(content.data()));
    }
    else
    {
        std::cout << pReply->errorString().toStdString() << std::endl;
    }

    pReply->deleteLater();

    m_curResult ++;
    if(m_curResult >= m_maxResults)
        QMessageBox::information(m_pMainWindow, "ОК", "Контент успешно загружен.");
}

void DataLoader::contentPerformed(QNetworkReply *pReply)
{
    if (pReply->error() == QNetworkReply::NoError)
    {
        QByteArray content = pReply->readAll();
        QTextCodec* pCodec = QTextCodec::codecForName("utf8");

        parseContent(pCodec->toUnicode(content.data()));
    }
    else
    {
        std::cout << pReply->errorString().toStdString() << std::endl;
    }

    pReply->deleteLater();

    m_curResult ++;
    if(m_curResult == m_maxResults)
    {
        QMessageBox::information(m_pMainWindow, "ОК", "Контент успешно сохранен.");
        m_out.close();
    }
}

const std::vector<QString> &DataLoader::resourceURLs()
{
    return m_aResourceURLs;
}

void DataLoader::fillSentences()
{
    disconnect(m_pManager, &QNetworkAccessManager::finished, this, &DataLoader::queryPerformed);
    connect(m_pManager, &QNetworkAccessManager::finished, this, &DataLoader::contentPerformed);

    m_out.open("parser//sentences.txt");
    m_curResult = 0;
    m_maxResults = static_cast<int>(m_aResourceURLs.size());

    for(const auto& url : m_aResourceURLs)
    {
        m_pManager->get(QNetworkRequest(url));
    }
}

void DataLoader::parseResponse(const QString& response)
{
    QRegExp pattern("<cite( class=\"[\\S ]+\")?>(\\S+)</cite>");
    int pos = 0;
    while((pos = pattern.indexIn(response, pos)) != -1)
    {
        QString url = pattern.cap(2);
        QString processedUrl;
        bool tagOpened = false;
        for(auto sym : url)
        {
            if(tagOpened)
            {
                if(sym == '>')
                    tagOpened = false;
            }
            else
            {
                if(sym == '<')
                    tagOpened = true;
                else
                    processedUrl += sym;
            }
        }

        m_aResourceURLs.push_back(processedUrl);
        pos += pattern.matchedLength();
    }
}

void DataLoader::parseContent(const QString& content)
{
    QTextDocument doc;
    doc.setHtml(content);
    m_out << doc.toPlainText().toStdString();
}

void DataLoader::get(int maxResults, const QString& url)
{
    connect(m_pManager, &QNetworkAccessManager::finished, this, &DataLoader::queryPerformed);
    disconnect(m_pManager, &QNetworkAccessManager::finished, this, &DataLoader::contentPerformed);

    if(maxResults < 0)
        return;

    m_maxResults = maxResults;
    m_curResult = 0;

    for(int ind = 0; ind < maxResults; ind ++)
    {
        QNetworkRequest request(url + "&start=" + std::to_string(ind * 10).c_str());
        m_pManager->get(request);
    }
}

void DataLoader::clearURLs()
{
    m_aResourceURLs.clear();
}
