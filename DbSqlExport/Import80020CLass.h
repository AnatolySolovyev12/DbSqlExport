#pragma once

#include <QMainWindow>
#include "ui_Import80020CLass.h"

class Import80020CLass : public QMainWindow
{
	Q_OBJECT

public:
	Import80020CLass(QWidget* parent = nullptr);

	~Import80020CLass();

private:
	Ui::Import80020CLassClass ui;

};

