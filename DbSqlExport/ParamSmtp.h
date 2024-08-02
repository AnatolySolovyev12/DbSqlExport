#pragma once

#include <QMainWindow>
#include "ui_ParamSmtp.h"

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include "SMTP.h"
#include <QFileDialog>
#include <SMTP.h>
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



private slots:
	void writeCurrent();
	
private:
	Ui::ParamSmtpClass ui;
	SMTP* mySMTP = nullptr;
	QString fileForSend;
};
