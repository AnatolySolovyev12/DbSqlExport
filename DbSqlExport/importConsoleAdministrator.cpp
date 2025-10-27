#include "importConsoleAdministrator.h"

importConsoleAdministrator::importConsoleAdministrator(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.importButton, &QPushButton::clicked, this, &importConsoleAdministrator::generateSignalForImport);
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	setTypeMeters();

	ui.progressBar->hide();
	barSharePtr = ui.progressBar;
	ui.textEdit->setReadOnly(true);
}


void importConsoleAdministrator::setTypeMeters()
{
	ui.listWidget->addItem("M2M-1");
	ui.listWidget->addItem("ÌÈËÓÐ-107 ÑÏÎÄÝÑ");
}


void importConsoleAdministrator::generateSignalForImport()
{
	if (ui.listWidget->currentItem() == nullptr) return;
	ui.progressBar->setValue(0);
	ui.progressBar->show();
	ui.importButton->hide();
	emit status(ui.listWidget->currentItem()->text());
	QTimer::singleShot(5000, [this]() {ui.importButton->show(); });
}


void importConsoleAdministrator::setCurRow()
{
	ui.listWidget->setCurrentRow(0);
}


QPointer<QProgressBar> importConsoleAdministrator::getPtrProgressBar()
{
	return barSharePtr;
}


void importConsoleAdministrator::printMessage(QString any)
{
	ui.textEdit->append(any + "\n");
}


void importConsoleAdministrator::clearTextEdit()
{
	ui.textEdit->clear();
}