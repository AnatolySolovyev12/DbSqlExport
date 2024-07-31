#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DbSqlExport.h"

#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>

#include <QSqlDatabase>



class DbSqlExport : public QMainWindow
{
    Q_OBJECT

public:
    DbSqlExport(QWidget *parent = nullptr);
    ~DbSqlExport();



    void addOneNumber();
    void removeNumber();
    void clearAllNumbers();
    void addSomeNumbers();
    void connectDataBase();
    void queryDbResult(QString any);
    void generateXml();
    void generalXmlLoop(QString any, QString day, QString night);

    QXmlStreamWriter xmlWriter;
    QSqlDatabase mw_db;

private:
    Ui::DbSqlExportClass ui;
    

    QString day;
    QString night;
    int countOfNumbers = 0;




};
