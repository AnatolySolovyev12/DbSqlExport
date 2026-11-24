#include "DbSqlExport.h"
#include <QtWidgets/QApplication>
#include <Windows.h>
#include <clocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DbSqlExport w;

    SetConsoleCP(65001);        // UTF‑8 вход
    SetConsoleOutputCP(65001);  // UTF‑8 вывод
    setlocale(LC_ALL, "ru_RU.UTF-8");

    w.setWindowTitle("DbSqlExport by Solovev");
    w.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "\\icon.png"));
    w.show();

    return a.exec();
}
