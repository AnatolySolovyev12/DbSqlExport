#include "Import80020CLass.h"

Import80020CLass::Import80020CLass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

Import80020CLass::~Import80020CLass()
{}

void Import80020CLass::setMaket(QString any)
{
	ui.listWidgetMaket80020->addItem(any);

}
