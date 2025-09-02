#pragma once

#include <QMainWindow>
#include "ui_Import80020CLass.h"
#include <QTimer>
#include <QProgressBar>
#include <QPointer>

class Import80020CLass : public QMainWindow
{
	Q_OBJECT

public:
	Import80020CLass(QWidget* parent = nullptr);

	void setMaket(QString any);
	void clearWidget();
	void generateSignalForImport();
	void setCurRow();
	QPointer<QProgressBar> getPtrProgressBar();

signals:
	void status(const QString&);

private:
	Ui::Import80020CLassClass ui;
	QPointer<QProgressBar> barSharePtr;
};

