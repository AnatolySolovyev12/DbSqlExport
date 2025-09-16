#include "importTreeObjectClass.h"

importTreeObjectClass::importTreeObjectClass(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.importButton, &QPushButton::clicked, this, &importTreeObjectClass::generateSignalForImport);
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.refImport, &QPushButton::clicked, this, &importTreeObjectClass::generateSignalForImportReference);

	ui.progressBar->hide();
	ui.checkBox->setChecked(true);
	ui.textEdit->setReadOnly(true);
}


QTreeWidget* importTreeObjectClass::returnWidget()
{
	return ui.treeWidget;
}


void importTreeObjectClass::generateSignalForImport()
{
	if (ui.treeWidget->currentItem() == nullptr) return;

	if (ui.treeWidget->currentItem()->text(2) == "144" || ui.treeWidget->currentItem()->text(2) == "143" || ui.treeWidget->currentItem()->text(2) == "141")
	{
		emit status(ui.treeWidget->currentItem()->text(1), ui.treeWidget->currentItem()->text(2));
		ui.importButton->hide();
		ui.refImport->hide();
		QTimer::singleShot(5000, [this]() {
			ui.importButton->show();
			ui.refImport->show();
			});
	}
	else
		return;
}


QPointer<QProgressBar> importTreeObjectClass::getPtrProgressBar()
{
	return ui.progressBar;
}


QPointer<QCheckBox> importTreeObjectClass::getPtrCheckBoxTariff()
{
	return ui.checkBox;
}


void importTreeObjectClass::printMessage(QString any)
{
	ui.textEdit->append(any + "\n");
}


void importTreeObjectClass::clearTextEdit()
{
	ui.textEdit->clear();
}


void importTreeObjectClass::generateSignalForImportReference()
{
	if (ui.treeWidget->currentItem() == nullptr) return;

	if (ui.treeWidget->currentItem()->text(2) == "144" || ui.treeWidget->currentItem()->text(2) == "143" || ui.treeWidget->currentItem()->text(2) == "141")
	{
		emit importReferenceSignal(ui.treeWidget->currentItem()->text(1), ui.treeWidget->currentItem()->text(2));
		ui.importButton->hide();
		ui.refImport->hide();
		QTimer::singleShot(5000, [this]() {
			ui.importButton->show(); 
			ui.refImport->show();
			});
	}
	else
		return;
}