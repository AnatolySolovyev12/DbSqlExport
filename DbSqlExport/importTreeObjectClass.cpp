#include "importTreeObjectClass.h"

importTreeObjectClass::importTreeObjectClass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.importButton, &QPushButton::clicked, this, &importTreeObjectClass::generateSignalForImport);
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	//barPointerPtr = ui.progressBar;
	ui.progressBar->hide();
	ui.checkBox->setChecked(true);

	//checkTariffPtr = ui.checkBox;
}


QTreeWidget* importTreeObjectClass::returnWidget()
{
	return ui.treeWidget;
}


void importTreeObjectClass::generateSignalForImport()
{
	if (ui.treeWidget->currentItem() == nullptr) return;

	if (ui.treeWidget->currentItem()->text(2) != "144") return;

	emit status(ui.treeWidget->currentItem()->text(1));
	ui.importButton->hide();
	QTimer::singleShot(5000, [this]() {ui.importButton->show(); });
}


QPointer<QProgressBar> importTreeObjectClass::getPtrProgressBar()
{
	return ui.progressBar;
}


QPointer<QCheckBox> importTreeObjectClass::getPtrCheckBoxTariff()
{
	return ui.checkBox;
}
