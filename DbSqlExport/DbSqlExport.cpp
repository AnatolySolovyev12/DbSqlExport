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



void DbSqlExport::addOneNumber() // ��������� �����
{
    QInputDialog inputDialog;

    QString c_text = inputDialog.getText(this, "Add number", "Number of counter", QLineEdit::Normal);
    QString c_textSimp = c_text.simplified(); // ������� ������� � ������ � � ����� � �������� ���������� ������� ������� �� ���������.

    if (!c_textSimp.isEmpty()) 
    {
        ui.listWidget->addItem(c_textSimp);
        int r = ui.listWidget->count() - 1; // count() - ���������� ��������� � ������
        ui.listWidget->setCurrentRow(r); //�������, �������� ������� ����������� ������� � ������� ������ setCurrentRow().
        countOfNumbers++;
    }
}



void DbSqlExport::removeNumber() // �������� �������� 
{ 

    int r = ui.listWidget->currentRow();

    if (r != -1) 
    {
        QListWidgetItem* item = ui.listWidget->takeItem(r);
        delete item;
        countOfNumbers--;
    }
}



void DbSqlExport::clearAllNumbers() // ����� clear() ������� ��� �������� �� ������� ������:
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

    int listDonor = sheetsDonor->property("Count").toInt(); // ��� ����� �������� ���������� ������ � ���������

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

    QAxObject* sheetDonor = sheetsDonor->querySubObject("Item(int)", listDonor);// ��� ���������� ���� � ������� ����� �������

    QAxObject*  usedRangeDonor = sheetDonor->querySubObject("UsedRange"); // ��� ����� �������� ���������� ����� � ���������
    QAxObject*  rowsDonor = usedRangeDonor->querySubObject("Rows");
    int countRowsDonor = rowsDonor->property("Count").toInt();

    QAxObject* usedRangeColDonor = sheetDonor->querySubObject("UsedRange"); // ��� ����� �������� ���������� �������� � ���������
    QAxObject* columnsDonor = usedRangeColDonor->querySubObject("Columns");
    int countColsDonor = columnsDonor->property("Count").toInt();

    QAxObject* cell = nullptr;

    for (int row = 1; row <= countRowsDonor; ++row) 
    {
        cell = sheetDonor->querySubObject("Cells(int,int)", row, 1); // ��� ��������� � ����� ������� ��������
        QString currentString = cell->property("Value").toString();

        if (!currentString.isEmpty())
        {
            ui.listWidget->addItem(currentString);
            int r = ui.listWidget->count() - 1;
            ui.listWidget->setCurrentRow(r); //�������, �������� ������� ����������� ������� � ������� ������ setCurrentRow().
            countOfNumbers++;
        }

    }

    delete cell;
    cell = nullptr;

    workbookDonor->dynamicCall("Close()"); // ����������� ���������� � ������ � Excel ����� ��������� ����� �b���� ������ � �������
    excelDonor->dynamicCall("Quit()");

    delete workbookDonor;
    delete excelDonor;

}

void DbSqlExport::connectDataBase()
{
    QSqlDatabase mw_db = QSqlDatabase::addDatabase("QODBC"); // ��� ������� ODBC � Windows ���������� ������� ���������������� DNS � ����������������� �������. ������ �� ����� ��������.

    mw_db.setHostName("10.86.142.47"); // ���� ��� ����� ��
    mw_db.setDatabaseName("DBTESTZ"); // ��������� ��� ����������������� DNS ������� ��� ������ � ������� �����.
    mw_db.setUserName("solexp");
    mw_db.setPassword("RootToor#");

	if (!mw_db.open()) // ��������� ��. ���� �� ��������� �� ����� false
	{
		QString any;
		QString any2;

		any = mw_db.lastError().databaseText(); // ���� ���-�� ����� �� ��� �� ����� ��� � ����������
		any2 = mw_db.lastError().driverText();

		qDebug() << "Cannot open database: " << mw_db.lastError();

		QFile file("LOG.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Append);
		QTextStream out(&file); // ����� ������������ ������ ���������� � ����

		// ��� ������ ������ � ���� ���������� �������� <<
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
	curDate = curDate.addDays(-1); // �� ��� � ���� �� ������� � �� �� �����. ������� �������� ���� �� ������� ���� ��� ������������ �������
	QTime curTime = QTime::currentTime();
	QString timeInQuery = curDate.toString("yyyy-MM-dd"); // ������������� ������ ���� ��� ��� � ��.

	queryString = "select IDOBJECT_PARENT from dbo.PROPERTIES where PROPERTY_VALUE = '" + any + "'"; // ����������� ������ ��� ID ��� ������ �������

	query.exec(queryString); // ���������� ������ �� ���������� �������

	query.next();

	int iD = query.value(0).toInt() - 1; // ID � ����������� �� ������� ������ ��� �� ������� �� ������ ��������.

	queryString = "select VALUE_METERING from dbo.METERINGS where  IDOBJECT = '" + any.setNum(iD) + "' AND IDTYPE_OBJECT = '1201001' AND IDOBJECT_AGGREGATE = '1' AND TIME_END = '" + timeInQuery + " 19:00:00.0000000' AND VALUE_METERING != '0'"; // ����������� ��������� ��� ������ ����� ����������

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
    int countTimer = 0; // ��� ��������� ������ ������� ������������ �� ����������
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

    QString savedFile = QFileDialog::getSaveFileName(0, "Save XML", fileName, "*.xml"); // � ��������� ��������� ����� ����� ��������� tr("Xml files (*.xml)"). ��� ����� ��� �������� � ��������� �������. ������.

    if (savedFile == "") return;

    qDebug() << "Total devices in the list: " << countOfNumbers;

    qDebug() << "Wait...";

    QFile file(savedFile);

    myParamForSmtp.fileNameSetter(savedFile);

    file.open(QIODevice::WriteOnly);

   // QXmlStreamWriter xmlWriter(&file); // �������������� ������ QXmlStreamWriter ������� �� ������ � ������� ����� ��������
    xmlWriter.setDevice(&file);
    xmlWriter.setAutoFormatting(true); // ���������� ��� ��������������� �������� �� ����� ������
    xmlWriter.setAutoFormattingIndent(2); // ����� ���������� �������� � ������� (�� ��������� 4)
    xmlWriter.writeStartDocument(); // ����� � ����� ��������� ���������

    xmlWriter.writeStartElement("message"); // ��������� ��������� ������� "��������" xml

    xmlWriter.writeAttribute("class", "80020*"); // ����������� �������� ������ ��������� ������� ��������

    xmlWriter.writeAttribute("version", "2");

    xmlWriter.writeAttribute("number", "1");

    xmlWriter.writeStartElement("datetime"); // ��������� ������ ������� � �.�.

    xmlWriter.writeStartElement("timestamp");

    QString timeInHead = (curDate.toString("yyyy.MM.dd")) + (curTime.toString("hh:mm:ss"));

    for (int i = 0; i < timeInHead.size(); i++)
    {
        if (timeInHead[i].isPunct())
            timeInHead.remove(i, 1);
    }

    xmlWriter.writeCharacters(timeInHead); //������� ����� ��������� � ��������� ��������

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

    mw_db.removeDatabase("DBTESTZ"); // ���������� ���������������� DNS � ODBC;

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


