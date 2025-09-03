#pragma once

#include <QMainWindow>
#include "ui_importTreeObjectClass.h"
#include <QTimer>
#include <QPointer>
#include <QProgressBar>

class importTreeObjectClass : public QMainWindow
{
	Q_OBJECT

public:
	importTreeObjectClass(QWidget *parent = nullptr);

	QTreeWidget* returnWidget();
	void generateSignalForImport();
	QPointer<QProgressBar> getPtrProgressBar();

signals:
	void status(const QString&);

private:
	Ui::importTreeObjectClassClass ui;

	QPointer<QProgressBar> barPointerPtr = nullptr;
};

