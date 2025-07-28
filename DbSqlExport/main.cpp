#include "DbSqlExport.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DbSqlExport w;

    w.setWindowTitle("DbSqlExport by Solovev");
    w.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "\\icon.png"));
    w.show();

    return a.exec();
}
