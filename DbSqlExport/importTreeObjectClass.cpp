#include "importTreeObjectClass.h"

importTreeObjectClass::importTreeObjectClass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.importButton, &QPushButton::clicked, this, &importTreeObjectClass::generateSignalForImport);
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));
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
