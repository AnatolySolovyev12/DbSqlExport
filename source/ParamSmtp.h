#pragma once

#include <QMainWindow>
#include "ui_ParamSmtp.h"
#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include "SMTP.h"
#include <QFileDialog>
#include <QString>

class ParamSmtp : public QMainWindow
{
	Q_OBJECT

public:
	ParamSmtp(QWidget *parent = nullptr);
	~ParamSmtp();

	void readDefaultConfig();
	void fileNameSetter(QString any);
	void sendMailfromButton();

	QString smtpServer;
	QString smtpPort;
	QString userName;
	QString password;
	QString recipantTo;
	QString subject;

	QString hostName;
	QString odbc;
	QString userNameDb;
	QString passDb;

	int timerTime;

signals:
	void status(const QString&);

private slots:
	void writeCurrent();
	
private:
	Ui::ParamSmtpClass ui;
	SMTP* mySMTP = nullptr;
	QString fileForSend;
};
