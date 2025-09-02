#pragma once

#include <QMainWindow>
#include "ui_importTreeObjectClass.h"
#include <QTimer>

class importTreeObjectClass : public QMainWindow
{
	Q_OBJECT

public:
	importTreeObjectClass(QWidget *parent = nullptr);

	QTreeWidget* returnWidget();
	void generateSignalForImport();

signals:
	void status(const QString&);

private:
	Ui::importTreeObjectClassClass ui;
};

