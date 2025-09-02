#include "Import80020CLass.h"

Import80020CLass::Import80020CLass(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.importButton, &QPushButton::clicked, this, &Import80020CLass::generateSignalForImport);
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	ui.progressBar->hide();
	barSharePtr = ui.progressBar;
}


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
	ui.progressBar->setValue(0);
	ui.progressBar->show();
	ui.importButton->hide();
	emit status(ui.listWidgetMaket80020->currentItem()->text());
	QTimer::singleShot(5000, [this]() {ui.importButton->show();});
}

void Import80020CLass::setCurRow()
{
	ui.listWidgetMaket80020->setCurrentRow(0);
}

QPointer<QProgressBar> Import80020CLass::getPtrProgressBar()
{
	return barSharePtr;
}
