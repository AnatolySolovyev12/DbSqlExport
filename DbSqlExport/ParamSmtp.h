#pragma once

#include <QMainWindow>
#include "ui_ParamSmtp.h"

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include "SMTP.h"
#include <QFileDialog>

class ParamSmtp : public QMainWindow
{
	Q_OBJECT

public:
	ParamSmtp(QWidget *parent = nullptr);
	~ParamSmtp();

private slots:
	void sendMailfromButton();
	void MessegeAboutMailSend(QString);
	void browse();

private:
	Ui::ParamSmtpClass ui;

	SMTP* mySMTP = nullptr;
	QStringList files;
};
