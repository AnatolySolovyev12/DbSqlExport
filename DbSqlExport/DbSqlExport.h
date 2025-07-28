#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DbSqlExport.h"
#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>
#include <QSqlDatabase>
#include "ParamSmtp.h"
#include <QTimer>

#include <QPair>
#include <Import80020Class.h>



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
    void generalXmlLoop(QString any, QString day, QString night, QString counterGuid, QString dateDay);
    void optionsSmtp();
    void checkDelAfterSend(int myState);
    
    void import80020();
    void importClassBirth();

    void processWriteInDb();

    QXmlStreamWriter xmlWriter;
    QSqlDatabase mw_db;

    ParamSmtp * myParamForSmtp;
    QString fileName;

private slots:
    void slotTimerAlarm();
    void timerUpdate();
    void MessegeAboutReconnectDb(QString);
 
private:
    Ui::DbSqlExportClass ui;
    
    bool boolSendAfterCreate = false;
    bool boolDelAfterSend = false;
    bool dbconnect = false;
    QString day;
    QString night;
    QString guid;
    QString dateDay;
    int countOfNumbers = 0;

    QTimer* timer;

    QStatusBar * sBar;
    QLabel* dbLabel;

    QList<QPair<QString, QString>>bufferFor80020Import;

    Import80020CLass* importClass = nullptr;
};
