#pragma once

#include <QMainWindow>
#include "ui_importConsoleAdministrator.h"
#include <QTimer>
#include <QProgressBar>
#include <QPointer>

class importConsoleAdministrator : public QMainWindow
{
	Q_OBJECT

public:
	importConsoleAdministrator(QWidget *parent = nullptr);

	void setTypeMeters();
	void generateSignalForImport();
	void setCurRow();
	QPointer<QProgressBar> getPtrProgressBar();
	void printMessage(QString any);
	void clearTextEdit();

signals:
	void status(const QString&);


private:
	Ui::importConsoleAdministratorClass ui;
	QPointer<QProgressBar> barSharePtr;
};

