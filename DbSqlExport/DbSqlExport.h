#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DbSqlExport.h"

#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>

#include <QSqlDatabase>

#include "ParamSmtp.h"



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
    void checkSendAfterCreate(int myState);



    void generalXmlLoop(QString any, QString day, QString night, QString counterGuid);

    void optionsSmtp();

    QXmlStreamWriter xmlWriter;
    QSqlDatabase mw_db;

    ParamSmtp myParamForSmtp;
    QString fileName;


private:
    Ui::DbSqlExportClass ui;
    
    bool boolSendAfterCreate = false;
    QString day;
    QString night;
    QString guid;
    int countOfNumbers = 0;
};
