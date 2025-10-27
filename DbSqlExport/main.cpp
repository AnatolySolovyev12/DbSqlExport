#include "DbSqlExport.h"
#include <QtWidgets/QApplication>
#include <Windows.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DbSqlExport w;

    SetConsoleOutputCP(1251);
    w.setWindowTitle("DbSqlExport by Solovev");
    w.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "\\icon.png"));
    w.show();


    QString serial = "241100010549480";
    qDebug() << serial.sliced(11).toInt() + 16;
    return a.exec();
}
