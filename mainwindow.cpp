#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "pugixml.hpp"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dataLoader(this)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{
    std::wstring text = ui->keywordsText->text().toStdWString();
    std::wstringstream ss;
    ss << text;
    QString url("https://www.google.com/search?q=");
    while(ss >> text)
    {
        m_aKeywords.push_back(QString::fromStdWString(text));
        url.append(QString::fromStdWString(text));
        if(!ss.eof())
            url.append('+');
    }

    m_dataLoader.get(ui->numContextsBox->value(), url);
}

void MainWindow::on_saveButton_clicked()
{
    std::ofstream out(ui->filePath->text().toStdString());
    if(!out.is_open())
    {
        QMessageBox::critical(this, "Ошибка", "Не удается открыть файл");
        return;
    }

    QProcess* pProcess = new QProcess(this);
    QString parser("parser\\parser.exe");
    QStringList arguments{"-lemmatizer", "0", "-lang", "russian", "-emit_morph", "0", "-render_tree", "0", "-i", "parser/sentences.txt", "-o", "parser/out.xml", "-d", "parser/dictionary.xml"};
    pProcess->start(parser, arguments);
    pProcess->waitForFinished(-1);

    auto strEqualNoCase = [](const QString& s1, const QString& s2)
    {
        return s1.compare(s2, Qt::CaseInsensitive) == 0;
    };

    pugi::xml_document doc;
    doc.load_file("parser\\out.xml");
    const auto parsing = doc.first_child();
    for(const auto& sentence : parsing.children())
    {
        long long mask = (1 << m_aKeywords.size()) - 1, cur = 0;
        for(const auto& token : sentence.child(L"tokens").children(L"token"))
        {
            auto word = QString::fromStdWString(token.child(L"word").text().as_string());
            auto lemma = QString::fromStdWString(token.child(L"lemma").text().as_string());
            for(unsigned i = 0; i < m_aKeywords.size(); i++)
            {
                if(strEqualNoCase(m_aKeywords[i], word) || strEqualNoCase(m_aKeywords[i], lemma))
                {
                    cur |= (1 << i);
                    break;
                }
            }
        }
        if(cur == mask)
            out << QString::fromStdWString(sentence.child(L"text").text().as_string()).toStdString() << std::endl;
    }

    out.close();
    QMessageBox::information(this, "Успешно", "Контексты успешно сохранены");
}

void MainWindow::on_saveContentButton_clicked()
{
    m_dataLoader.fillSentences();
}
