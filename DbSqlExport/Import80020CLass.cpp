#include "Import80020CLass.h"

Import80020CLass::Import80020CLass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.listWidgetMaket80020->currentItem() == nullptr;
	connect(ui.importButton, &QPushButton::clicked, this, &Import80020CLass::generateSignalForImport);
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

Import80020CLass::~Import80020CLass()
{}

void Import80020CLass::setMaket(QString any)
{
	ui.listWidgetMaket80020->addItem(any);

}

void Import80020CLass::clearWidget()
{
	ui.listWidgetMaket80020->clear();
}

void Import80020CLass::generateSignalForImport()
{
	if (ui.listWidgetMaket80020->currentItem() == nullptr) return;
	emit status(ui.listWidgetMaket80020->currentItem()->text());
	ui.importButton->hide();
	QTimer::singleShot(5000, [this]() {ui.importButton->show(); });
}
