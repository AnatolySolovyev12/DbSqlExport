#pragma once

#include <QMainWindow>
#include "ui_importTreeObjectClass.h"
#include <QTimer>
#include <QPointer>

class importTreeObjectClass : public QMainWindow
{
	Q_OBJECT

public:
	importTreeObjectClass(QWidget *parent = nullptr);

	QTreeWidget* returnWidget();
	void generateSignalForImport();
	QPointer<QProgressBar> getPtrProgressBar();

	QPointer<QCheckBox> getPtrCheckBoxTariff();

signals:
	void status(const QString&, const QString&);

private:
	Ui::importTreeObjectClassClass ui;
};

