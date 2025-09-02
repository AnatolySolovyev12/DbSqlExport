#include "importTreeObjectClass.h"

importTreeObjectClass::importTreeObjectClass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

QTreeWidget* importTreeObjectClass::returnWidget()
{
	return ui.treeWidget;
}
