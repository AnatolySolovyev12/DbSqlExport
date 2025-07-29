#pragma once

#include <QMainWindow>
#include "ui_Import80020CLass.h"
#include <QTimer>

class Import80020CLass : public QMainWindow
{
	Q_OBJECT

public:
	Import80020CLass(QWidget* parent = nullptr);

	~Import80020CLass();

	void setMaket(QString any);
	void clearWidget();
	void generateSignalForImport();
	void setCurRow();

signals:
	void status(const QString&);

private:
	Ui::Import80020CLassClass ui;

};

