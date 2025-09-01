#pragma once

#include <QMainWindow>
#include "ui_importTreeObjectClass.h"

class importTreeObjectClass : public QMainWindow
{
	Q_OBJECT

public:
	importTreeObjectClass(QWidget *parent = nullptr);
	~importTreeObjectClass();

private:
	Ui::importTreeObjectClassClass ui;
};

