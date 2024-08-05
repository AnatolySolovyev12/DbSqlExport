#include "DbSqlExport.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QAxObject>

#include <QFile>

#include <QSqlError>
#include <QSqlQuery>
#include <QTime>

#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>

#include <QElapsedTimer>
#include <QtNetwork/QSslSocket>

QTextStream out(stdout);
QTextStream in(stdin);


DbSqlExport::DbSqlExport(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButtonAddNumber, &QPushButton::clicked, this, &DbSqlExport::addOneNumber);
    connect(ui.pushButtonDeleteNumber, &QPushButton::clicked, this, &DbSqlExport::removeNumber);
    connect(ui.pushButtonDeleteAll, &QPushButton::clicked, this, &DbSqlExport::clearAllNumbers);
    connect(ui.pushButtonAddFrom, &QPushButton::clicked, this, &DbSqlExport::addSomeNumbers);
    connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui.pushButtonGenXml, &QPushButton::clicked, this, &DbSqlExport::generateXml);
    connect(ui.pushButtonSendFiles, &QPushButton::clicked, this, &DbSqlExport::optionsSmtp);

    connect(ui.checkBoxSendAfterCreate, &QCheckBox::stateChanged, this, &DbSqlExport::checkSendAfterCreate);

    
}

DbSqlExport::~DbSqlExport()
{}



void DbSqlExport::addOneNumber() // добавляем номер
{
    QInputDialog inputDialog;

    QString c_text = inputDialog.getText(this, "Add number", "Number of counter", QLineEdit::Normal);
    QString c_textSimp = c_text.simplified(); // удаляем проблеы в начале и в конце и заменяем внутренние двойные пробелы на одинарные.

    if (!c_textSimp.isEmpty()) 
    {
        ui.listWidget->addItem(c_textSimp);
        int r = ui.listWidget->count() - 1; // count() - количество элементов в списке
        ui.listWidget->setCurrentRow(r); //наконец, выделяем текущий добавленный элемент с помощью метода setCurrentRow().
        countOfNumbers++;
    }
}



void DbSqlExport::removeNumber() // удаление элемента 
{ 

    int r = ui.listWidget->currentRow();

    if (r != -1) 
    {
        QListWidgetItem* item = ui.listWidget->takeItem(r);
        delete item;
        countOfNumbers--;
    }
}



void DbSqlExport::clearAllNumbers() // метод clear() удаляет все элементы из виджета списка:
{ 


    if (ui.listWidget->count() != 0)
    {
        ui.listWidget->clear();
        countOfNumbers = 0;
    }
}



void DbSqlExport::addSomeNumbers() 
{
    QString addFileDonor = QFileDialog::getOpenFileName(0, "Add list of numbers", "", "*.xls *.xlsx");
    
    if (addFileDonor == "")
    {
        return;
    }

    QAxObject * excelDonor = new QAxObject("Excel.Application", 0);
    QAxObject* workbooksDonor = excelDonor->querySubObject("Workbooks");
    QAxObject* workbookDonor = workbooksDonor->querySubObject("Open(const QString&)", addFileDonor); // 
    QAxObject* sheetsDonor = workbookDonor->querySubObject("Worksheets");

    int listDonor = sheetsDonor->property("Count").toInt(); // так можем получить количество листов в документе

    if (listDonor > 1)
    {
        do
        {
            listDonor = QInputDialog::getInt(this, "List nomber", "Whats list do you need?");

            if (!listDonor)
            {
                return;
            }

        } while (listDonor <= 0 || (listDonor > (sheetsDonor->property("Count").toInt())));

    }

    QAxObject* sheetDonor = sheetsDonor->querySubObject("Item(int)", listDonor);// Тут определяем лист с которым будем работаь

    QAxObject*  usedRangeDonor = sheetDonor->querySubObject("UsedRange"); // так можем получить количество строк в документе
    QAxObject*  rowsDonor = usedRangeDonor->querySubObject("Rows");
    int countRowsDonor = rowsDonor->property("Count").toInt();

    QAxObject* usedRangeColDonor = sheetDonor->querySubObject("UsedRange"); // так можем получить количество столбцов в документе
    QAxObject* columnsDonor = usedRangeColDonor->querySubObject("Columns");
    int countColsDonor = columnsDonor->property("Count").toInt();

    QAxObject* cell = nullptr;

    for (int row = 1; row <= countRowsDonor; ++row) 
    {
        cell = sheetDonor->querySubObject("Cells(int,int)", row, 1); // так указываем с какой ячейкой работать
        QString currentString = cell->property("Value").toString();

        if (!currentString.isEmpty())
        {
            ui.listWidget->addItem(currentString);
            int r = ui.listWidget->count() - 1;
            ui.listWidget->setCurrentRow(r); //наконец, выделяем текущий добавленный элемент с помощью метода setCurrentRow().
            countOfNumbers++;
        }

    }

    delete cell;
    cell = nullptr;

    workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
    excelDonor->dynamicCall("Quit()");

    delete workbookDonor;
    delete excelDonor;

}

void DbSqlExport::connectDataBase()
{
    QSqlDatabase mw_db = QSqlDatabase::addDatabase("QODBC"); // Для раблоты ODBC в Windows необходимо задвать пользовательский DNS в администрировании системы. Иначен не будет работать.

    mw_db.setHostName("10.86.142.47"); // хост где лежит БД
    mw_db.setDatabaseName("DBTESTZ"); // указываем имя пользовательского DNS который был создан в системе ранее.
    mw_db.setUserName("solexp");
    mw_db.setPassword("RootToor#");

	if (!mw_db.open()) // открываем БД. Если не открывает то вернёт false
	{
		QString any;
		QString any2;

		any = mw_db.lastError().databaseText(); // если что-то пойдёт не так то пишем это в переменные
		any2 = mw_db.lastError().driverText();

		qDebug() << "Cannot open database: " << mw_db.lastError();

		QFile file("LOG.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Append);
		QTextStream out(&file); // поток записываемых данных направляем в файл

		// Для записи данных в файл используем оператор <<
		out << any << Qt::endl;
		out << any2 << Qt::endl;

		file.close();
	}
	else
		qDebug() << "DataBase is CONNECT.";


    //mw_db.removeDatabase("DBTESTZ");
}

void DbSqlExport::queryDbResult(QString any)
{
    QSqlQuery query;
    QString queryString;

	QDate curDate = QDate::currentDate();
	curDate = curDate.addDays(-1); // то что в Заре на сегодня в БД на вчера. Поэтому вычетаем день от текущей даты для последующего запроса
	QTime curTime = QTime::currentTime();
	QString timeInQuery = curDate.toString("yyyy-MM-dd"); // Разворачиваем формат даты так как в БД.

	queryString = "select IDOBJECT_PARENT from dbo.PROPERTIES where PROPERTY_VALUE = '" + any + "'"; // запрашиваем нужный нам ID поо номеру прибора

	query.exec(queryString); // Отправляем запрос на количество записей

	query.next();

	int iD = query.value(0).toInt() - 1; // ID с показаниями на единицу меньше чем мы выявили по номеру счётчика.

	queryString = "select VALUE_METERING from dbo.METERINGS where  IDOBJECT = '" + any.setNum(iD) + "' AND IDTYPE_OBJECT = '1201001' AND IDOBJECT_AGGREGATE = '1' AND TIME_END = '" + timeInQuery + " 19:00:00.0000000' AND VALUE_METERING != '0'"; // запрашиваем показаний без всякой лишей информации

	query.exec(queryString);

	query.next();

	day = query.value(0).toString();

	queryString = "select VALUE_METERING from dbo.METERINGS where  IDOBJECT = '" + any.setNum(iD) + "' AND IDTYPE_OBJECT = '1202001' AND IDOBJECT_AGGREGATE = '1' AND TIME_END = '" + timeInQuery + " 19:00:00.0000000' AND VALUE_METERING != '0'";

	query.exec(queryString);

	query.next();

	night = query.value(0).toString();

    queryString = "select IDOBJECT_FROM from dbo.LINK_OBJECTS where IDOBJECT_TO = '" + any.setNum(iD+1) + "' AND IDTYPE_OBJECT_LINK = '1000011'";

    query.exec(queryString);

    query.next();

    iD = query.value(0).toInt();

    queryString = "select PROPERTY_VALUE from PROPERTIES where IDOBJECT_PARENT = '" + any.setNum(iD) + "' and IDTYPE_PROPERTY = '939'";

    query.exec(queryString);

    query.next();

    guid = query.value(0).toString(); 
}

void DbSqlExport::generateXml()
{
    QElapsedTimer timer;
    int countTimer = 0; // для итогового вывода времени потраченного на выполнение
    timer.start();

    connectDataBase();

    QDate curDate = QDate::currentDate();
    QTime curTime = QTime::currentTime();

    fileName = "80020__" + (curDate.toString("dd.MM.yyyy")) + "__" + (curTime.toString("hh:mm:ss"));

    for (int i = 0; i < fileName.size(); i++)
    {
        if (fileName[i].isPunct())
            fileName.remove(i, 1);
    }

    QString savedFile = QFileDialog::getSaveFileName(0, "Save XML", fileName, "*.xml"); // В последнем параметре также можно прописать tr("Xml files (*.xml)"). Это будет как приписка с указанием формата. Удобно.

    if (savedFile == "") return;

    qDebug() << "Total devices in the list: " << countOfNumbers;

    qDebug() << "Wait...";

    QFile file(savedFile);

    myParamForSmtp.fileNameSetter(savedFile);

    file.open(QIODevice::WriteOnly);

   // QXmlStreamWriter xmlWriter(&file); // инициализируем объект QXmlStreamWriter ссылкой на объект с которым будем работать
    xmlWriter.setDevice(&file);
    xmlWriter.setAutoFormatting(true); // необходимо для автоматического перехода на новую строку
    xmlWriter.setAutoFormattingIndent(2); // задаём количество пробелов в отступе (по умолчанию 4)
    xmlWriter.writeStartDocument(); // пишет в шапке кодировку документа

    xmlWriter.writeStartElement("message"); // отркывает начальный элемент "лестницы" xml

    xmlWriter.writeAttribute("class", "80020*"); // присваиваем атрибуты внутри открытого первого элемента

    xmlWriter.writeAttribute("version", "2");

    xmlWriter.writeAttribute("number", "1");

    xmlWriter.writeStartElement("datetime"); // отркывает второй элемент и т.д.

    xmlWriter.writeStartElement("timestamp");

    QString timeInHead = (curDate.toString("yyyy.MM.dd")) + (curTime.toString("hh:mm:ss"));

    for (int i = 0; i < timeInHead.size(); i++)
    {
        if (timeInHead[i].isPunct())
            timeInHead.remove(i, 1);
    }

    xmlWriter.writeCharacters(timeInHead); //вставка между открытием и закрытием элемента

    xmlWriter.writeEndElement(); // timestamp

    xmlWriter.writeStartElement("daylightsavingtime");

    xmlWriter.writeCharacters("0");

    xmlWriter.writeEndElement(); // daylightsavingtime

    xmlWriter.writeStartElement("day");

    QString dateInHead = (curDate.toString("yyyy.MM.dd"));

    for (int i = 0; i < dateInHead.size(); i++)
    {
        if (dateInHead[i].isPunct())
            dateInHead.remove(i, 1);
    }


    xmlWriter.writeCharacters(dateInHead);

    xmlWriter.writeEndElement(); // day

    xmlWriter.writeEndElement(); // datetime

    xmlWriter.writeStartElement("sender");

    xmlWriter.writeStartElement("inn");

    xmlWriter.writeCharacters("8603103354");

    xmlWriter.writeEndElement(); // inn

    xmlWriter.writeStartElement("name");

    xmlWriter.writeCharacters("Gorsvet");

    xmlWriter.writeEndElement(); // name

    xmlWriter.writeEndElement(); // sender

    xmlWriter.writeStartElement("area");

    xmlWriter.writeStartElement("inn");

    xmlWriter.writeCharacters("8602067215");

    xmlWriter.writeEndElement(); // inn2

    xmlWriter.writeStartElement("name");

    xmlWriter.writeCharacters("Schetchiki");

    xmlWriter.writeEndElement(); // name3

    for (int i = 0; i < countOfNumbers; i++)
    {
        ui.listWidget->setCurrentRow(i);

        queryDbResult(ui.listWidget->currentItem()->text());

        generalXmlLoop(ui.listWidget->currentItem()->text(), day, night, guid);

    }

    xmlWriter.writeEndElement(); // area

    xmlWriter.writeEndElement(); // message

    xmlWriter.writeEndDocument();

    file.close();

    countTimer = timer.elapsed();

    out << "XML was made for = " << (double)countTimer / 1000 << " sec" << Qt::endl;

    mw_db.removeDatabase("DBTESTZ"); // Подключаем пользовательский DNS с ODBC;

    if(boolSendAfterCreate)
        myParamForSmtp.sendMailfromButton();

    fileName = "";
}


void DbSqlExport::generalXmlLoop(QString any, QString dayFunc, QString nightFunc, QString counterGuid)
{
	QString desc = "0";

	xmlWriter.writeStartElement("measuringpoint");

	xmlWriter.writeAttribute("code", counterGuid);

	xmlWriter.writeAttribute("name", any);

	xmlWriter.writeAttribute("serial", any);

	for (int internalCounter = 0; internalCounter < 3; internalCounter++)
	{
		xmlWriter.writeStartElement("measuringchannel");

		xmlWriter.writeAttribute("code", "01");

		if (internalCounter == 1) desc = "9";
		if (internalCounter == 2) desc = "10";


		xmlWriter.writeAttribute("desc", desc);

		xmlWriter.writeStartElement("period");


		xmlWriter.writeAttribute("start", "0000");

		xmlWriter.writeAttribute("end", "0000");

		xmlWriter.writeStartElement("timestamp");

		QString curDate = (QDate::currentDate().toString("yyyy.MM.dd"));


        for (int i = 0; i < curDate.size(); i++)
        {
            if (curDate[i].isPunct())
                curDate.remove(i, 1);
        }

        if (desc == "10") curDate = "189912300200";
        else curDate += "0000";

        if (dayFunc == "")
        {
            day = "0";
            curDate = "189912300200";
        }
        if (nightFunc == "")
        {
            night = "0";
            curDate = "189912300200";
        }

		xmlWriter.writeCharacters(curDate);

		xmlWriter.writeEndElement(); // timestamp

		xmlWriter.writeStartElement("value");



		if (internalCounter == 0) xmlWriter.writeCharacters(day);
		if (internalCounter == 1) xmlWriter.writeCharacters(night);
        if (internalCounter == 2) xmlWriter.writeCharacters("0");

		xmlWriter.writeEndElement(); // value

		xmlWriter.writeEndElement(); // period

		xmlWriter.writeEndElement(); // measurechannel

		desc = "0";

       // continue;
	}

	xmlWriter.writeEndElement(); // measurepoint

}

void DbSqlExport::optionsSmtp()
{
    myParamForSmtp.show();
    myParamForSmtp.readDefaultConfig();
}

void DbSqlExport::checkSendAfterCreate(int myState) {

    if (myState == Qt::Checked) {
        boolSendAfterCreate = true;
    }
    else {
        boolSendAfterCreate = false;
    }
}


