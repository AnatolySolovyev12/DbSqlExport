#include "DbSqlExport.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QAxObject>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

#include <QSqlRecord>

#include <QTime>
#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>
#include <QElapsedTimer>
#include <QtNetwork/QSslSocket>

QTextStream out(stdout);
QTextStream in(stdin);

DbSqlExport::DbSqlExport(QWidget* parent)
	: QMainWindow(parent), importClass(new Import80020CLass())
{
	ui.setupUi(this);

	myParamForSmtp = new ParamSmtp();

	connect(ui.pushButtonAddNumber, &QPushButton::clicked, this, &DbSqlExport::addOneNumber);
	connect(ui.pushButtonDeleteNumber, &QPushButton::clicked, this, &DbSqlExport::removeNumber);
	connect(ui.pushButtonDeleteAll, &QPushButton::clicked, this, &DbSqlExport::clearAllNumbers);
	connect(ui.pushButtonAddFrom, &QPushButton::clicked, this, &DbSqlExport::addSomeNumbers);
	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.pushButtonGenXml, &QPushButton::clicked, this, &DbSqlExport::generateXml);
	connect(ui.importDbButton, &QPushButton::clicked, this, &DbSqlExport::import80020);
	connect(ui.pushButtonSendFiles, &QPushButton::clicked, this, &DbSqlExport::optionsSmtp);

	connect(ui.checkBoxSendAfterCreate, &QCheckBox::stateChanged, this, &DbSqlExport::checkSendAfterCreate);
	connect(ui.checkBoxDelAfterSend, &QCheckBox::stateChanged, this, &DbSqlExport::checkDelAfterSend);

	connect(ui.autoSender, SIGNAL(pressed()), this, SLOT(timerUpdate()));

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
	timer->start(myParamForSmtp->timerTime * 3600000); // Каждые три секунды

	connect(myParamForSmtp, SIGNAL(status(QString)), this, SLOT(MessegeAboutReconnectDb(QString))); // делаем реконект к БД после каждого сохранения настроек.

	connect(importClass, SIGNAL(status(QString)), this, SLOT(processWriteInDb(QString))); // делаем реконект к БД после каждого сохранения настроек.

	sBar = new QStatusBar();
	QMainWindow::setStatusBar(sBar);
	sBar->showMessage("Autocreate was stoped");

	dbLabel = new QLabel();
	sBar->addPermanentWidget(dbLabel);
	dbLabel->setStyleSheet("color: rgb(255, 0, 0)");

	sBar->setStyleSheet("QStatusBar::item {border: None;}");

	connectDataBase();
}

DbSqlExport::~DbSqlExport()
{
}


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

	QAxObject* excelDonor = new QAxObject("Excel.Application", 0);
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

	QAxObject* sheetDonor = sheetsDonor->querySubObject("Item(int)", listDonor);// Тут определяем лист с которым будем работать

	QAxObject* usedRangeDonor = sheetDonor->querySubObject("UsedRange"); // так можем получить количество строк в документе
	QAxObject* rowsDonor = usedRangeDonor->querySubObject("Rows");
	int countRowsDonor = rowsDonor->property("Count").toInt();

	QAxObject* usedRangeColDonor = sheetDonor->querySubObject("UsedRange"); // так можем получить количество столбцов в документе
	QAxObject* columnsDonor = usedRangeColDonor->querySubObject("Columns");
	int countColsDonor = columnsDonor->property("Count").toInt();

	QAxObject* cell = nullptr;

	for (int row = 1; row <= countRowsDonor; ++row)
	{
		cell = sheetDonor->querySubObject("Cells(int,int)", row, 1); // так указываем с какой ячейкой работать
		QString currentString = cell->property("Value").toString().trimmed();

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

	if (myParamForSmtp->hostName == "")
	{
		mw_db.setDatabaseName(myParamForSmtp->odbc); // указываем имя пользовательского DNS который был создан в системе ранее.
		mw_db.setUserName(myParamForSmtp->userNameDb);
		mw_db.setPassword(myParamForSmtp->passDb);
	}
	else
	{
		QString dbParam = "DRIVER={SQL Server};SERVER=" + myParamForSmtp->hostName + ";DATABASE=" + myParamForSmtp->odbc + ";UID=" + myParamForSmtp->userNameDb + ";PWD=" + myParamForSmtp->passDb + ";";
		mw_db.setDatabaseName(dbParam);
	}

	//mw_db.setDatabaseName("DRIVER={SQL Server};SERVER=10.86.142.14;DATABASE=ProSoft_ASKUE;UID=solexp;PWD=RootToor#;");

	if (!mw_db.open()) // открываем БД. Если не открывает то вернёт false
	{
		QString any;
		QString any2;

		any = mw_db.lastError().databaseText(); // если что-то пойдёт не так то пишем это в переменные
		any2 = mw_db.lastError().driverText();

		qDebug() << "Cannot open database: " << mw_db.lastError();

		QFile file(QCoreApplication::applicationDirPath() + "\\LOG.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Append);
		QTextStream out(&file); // поток записываемых данных направляем в файл

		// Для записи данных в файл используем оператор <<
		out << any << Qt::endl;
		out << any2 << Qt::endl;

		file.close();
		dbconnect = false;
		dbLabel->setText("SQL Disconnected");
		dbLabel->setStyleSheet("color: rgb(255, 0, 0)");
	}
	else
	{
		qDebug() << "DataBase is CONNECT.";
		dbconnect = true;
		dbLabel->setText("SQL Connected");
		dbLabel->setStyleSheet("color: rgb(0, 255, 0)");
	}
}


void DbSqlExport::queryDbResult(QString any)
{
	QSqlQuery query;
	QString queryString;

	int iD = 0;

	int guidId;

	if (myParamForSmtp->odbc == "DBZS" || myParamForSmtp->odbc == "DBZM")
	{
		QDate curDate = QDate::currentDate();
		curDate = curDate.addDays(-1); // то что в Заре на сегодня в БД на вчера. Поэтому вычетаем день от текущей даты для последующего запроса
		QString timeInQuery = curDate.toString("yyyy-MM-dd"); // Разворачиваем формат даты так как в БД.

		queryString = "select IDOBJECT_PARENT from dbo.PROPERTIES where PROPERTY_VALUE = '" + any + "' and IDTYPE_PROPERTY = '987' ORDER BY IDOBJECT_PARENT DESC"; // запрашиваем нужный нам ID поо номеру прибора

		query.exec(queryString); // Отправляем запрос на количество записей

		query.next();

		iD = query.value(0).toInt(); // ID с показаниями на единицу меньше чем мы выявили по номеру счётчика.

		guidId = iD;

		queryString = "select IDOBJECT_TO from dbo.LINK_OBJECTS where IDOBJECT_FROM = '" + any.setNum(iD) + "' and IDTYPE_OBJECT_LINK = '1000011'";

		query.exec(queryString);
		query.next();

		if (query.isNull(0))
		{
			queryString = "select IDOBJECT_TO from dbo.LINK_OBJECTS where IDOBJECT_FROM = '" + any.setNum(iD) + "' ORDER BY IDLINK_OBJECTS DESC";
			query.exec(queryString);
			query.next();
		}

		iD = query.value(0).toInt(); // ID с показаниями на единицу меньше чем мы выявили по номеру счётчика.

		queryString = "select TOP 1 VALUE_METERING, FORMAT(DATEADD(DAY, 1 ,TIME_END), 'yyyy.MM.dd') as TIME_END from dbo.METERINGS where  IDOBJECT = '" + any.setNum(iD) + "' AND IDTYPE_OBJECT = '1201001' AND IDOBJECT_AGGREGATE = '1'  AND VALUE_METERING != '0' order by TIME_END DESC"; // запрашиваем показаний без всякой лишей информации

		query.exec(queryString);

		query.next();

		day = query.value(0).toString();

		if (day.length() >= 14) //исправляем ошибку переноса из БД в строку при которой добавляется куча значений после запятой
			day.chop(9);

		dateDay = query.value(1).toString();

		queryString = "select TOP 1 VALUE_METERING, FORMAT(DATEADD(DAY, 1 ,TIME_END), 'yyyy.MM.dd') as TIME_END from dbo.METERINGS where  IDOBJECT = '" + any.setNum(iD) + "' AND IDTYPE_OBJECT = '1202001' AND IDOBJECT_AGGREGATE = '1'  AND VALUE_METERING != '0' order by TIME_END DESC";

		query.exec(queryString);

		query.next();

		night = query.value(0).toString();

		if (night.length() >= 14)
			night.chop(9);

		queryString = "select IDOBJECT_FROM from dbo.LINK_OBJECTS where IDOBJECT_TO = '" + any.setNum(guidId) + "' AND IDTYPE_OBJECT_LINK = '1000011'";

		query.exec(queryString);

		query.next();

		guidId = query.value(0).toInt();

		queryString = "select PROPERTY_VALUE from PROPERTIES where IDOBJECT_PARENT = '" + any.setNum(guidId) + "' and IDTYPE_PROPERTY = '939'";

		query.exec(queryString);

		query.next();

		guid = query.value(0).toString();
	}

	if (myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEN" || myParamForSmtp->odbc == "DBEY")
	{
		QDate curDate = QDate::currentDate();

		if (myParamForSmtp->odbc == "DBEG")
			curDate = curDate.addDays(-1);

		QString timeInQuery = curDate.toString("yyyy-MM-dd"); // Разворачиваем формат даты так как в БД.

		queryString = "select ID_MeterInfo from MeterInfo where SN = '" + any + "'"; // запрашиваем первичный ID по номеру прибора
		query.exec(queryString);

		query.next();

		iD = query.value(0).toInt();

		queryString = "select ID_Point from MeterMountHist where ID_MeterInfo = '" + any.setNum(iD) + "'"; // получаем ID из счётчика

		query.exec(queryString);
		query.next();
		iD = query.value(0).toInt();

		queryString = "select * from dbo.PointParams where ID_Point = '" + any.setNum(iD) + "' and ID_Param = '4'"; // получаем ID параметра активной энергии счётчика
		query.exec(queryString);
		query.next();

		iD = query.value(0).toInt();

		if (myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEY")
			queryString = "select Val, FORMAT(DT+1, 'yyyy.MM.dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '1' order by DT DESC";

		if (myParamForSmtp->odbc == "DBEN")
			queryString = "select Val, FORMAT(DT, 'yyyy.MM.dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '1' order by DT DESC";

		query.exec(queryString);
		query.next();
		day = query.value(0).toString();

		if (day.length() >= 14)
			day.chop(9);

		dateDay = query.value(1).toString();

		if (myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEY")
			queryString = "select Val, FORMAT(DT+1, 'yyyy.MM.dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '2' order by DT DESC";

		if (myParamForSmtp->odbc == "DBEN")
			queryString = "select Val, FORMAT(DT, 'yyyy.MM.dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '2' order by DT DESC";

		query.exec(queryString);
		query.next();
		night = query.value(0).toString();

		if (night.length() >= 14)
			night.chop(9);

		queryString = "select ID_Parent from NDIETable where ID_PP = '" + any.setNum(iD) + "'"; // получаем ID для последующего получаения GUID
		query.exec(queryString);
		query.next();
		iD = query.value(0).toInt();

		queryString = "select Code from NDIETable where ID_DIE = '" + any.setNum(iD) + "'"; // получаем GUID
		query.exec(queryString);
		query.next();
		guid = query.value(0).toString();
	}

	if (myParamForSmtp->odbc == "DBKV")
	{
		QDate curDate = QDate::currentDate();

		QString timeInQuery = curDate.toString("yyyy-MM-dd"); // Разворачиваем формат даты так как в БД.

		queryString = "select id from [LERS].[dbo].[Equipment] where SerialNumber = '" + any + "' order by id DESC"; // запрашиваем первичный ID по номеру прибора

		query.exec(queryString);
		query.next();

		iD = query.value(0).toInt();

		queryString = "select MeasurePointID from [LERS].[dbo].[DeviceMeasurePoint] where EquipmentId = '" + any.setNum(iD) + "'"; // получаем ID из ID

		query.exec(queryString);
		query.next();

		iD = query.value(0).toInt();

		queryString = "select TOP(1) FORMAT(DataDate, 'yyyy.MM.dd') as DataDate, Ap1, Ap2 from [LERS].[dbo].[ElectricTotals] where  MeasurePointId = '" + any.setNum(iD) + "' order by DataDate DESC";

		query.exec(queryString);
		query.next();

		day = query.value(1).toString();

		if (day.length() >= 18)
			day.chop(9);

		night = query.value(2).toString();

		if (night.length() >= 18)
			night.chop(9);

		dateDay = query.value(0).toString();

		queryString = "select MeasurePoint_Comment, PersonalAccountID from [LERS].[dbo].[MeasurePoint] where MeasurePoint_ID = '" + any.setNum(iD) + "'"; // получаем ID для последующего получаения GUID
		query.exec(queryString);
		query.next();

		iD = query.value(1).toInt();

		if (!iD)
			guid = query.value(0).toString();
		else
		{
			queryString = "select PersonalAccount_Number from [LERS].[dbo].[PersonalAccount] where PersonalAccount_ID = '" + any.setNum(iD) + "'"; // получаем ID для последующего получаения GUID
			query.exec(queryString);
			query.next();

			guid = query.value(0).toString();
		}
	}
}


void DbSqlExport::generateXml()
{
	QElapsedTimer timer;
	int countTimer = 0; // для итогового вывода времени потраченного на выполнение
	int countDoingIterationForTime = 0; // считаем количество выполнений
	int valueForTimer = 5000; // временной отрезок для подсчёта количества выполнений
	timer.start();

	QDate curDate = QDate::currentDate();
	QTime curTime = QTime::currentTime();

	fileName = "80020__" + (curDate.toString("dd.MM.yyyy")) + "__" + (curTime.toString("hh:mm:ss"));

	for (int i = 0; i < fileName.size(); i++)
	{
		if (fileName[i].isPunct())
			fileName.remove(i, 1);
	}

	QString savedFile;

	if (!ui.autoSender->isChecked())
		savedFile = QFileDialog::getSaveFileName(0, "Save XML", fileName, "*.xml"); // В последнем параметре также можно прописать tr("Xml files (*.xml)"). Это будет как приписка с указанием формата. Удобно.
	else
		savedFile = fileName + ".xml";

	if (savedFile == "") return;

	qDebug() << "\nTotal devices in the list: " << countOfNumbers;

	qDebug() << "Wait...";

	QFile file(savedFile);

	myParamForSmtp->fileNameSetter(savedFile);

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
		++countDoingIterationForTime; // счётчик количества выполнений за единицу времени

		ui.listWidget->setCurrentRow(i);

		queryDbResult(ui.listWidget->currentItem()->text());

		generalXmlLoop(ui.listWidget->currentItem()->text(), day, night, guid, dateDay);

		if (valueForTimer - timer.elapsed() <= 100) // для отслеживания количества выполнений каждые 5 секунд.
		{
			valueForTimer += 5000;

			QTime ct = QTime::currentTime(); // возвращаем текущее время

			qDebug() << ct.toString() << "   " << countDoingIterationForTime;

			countDoingIterationForTime = 0;
		}
	}

	xmlWriter.writeEndElement(); // area

	xmlWriter.writeEndElement(); // message

	xmlWriter.writeEndDocument();

	file.close();

	countTimer = timer.elapsed();

	out << "XML was made for = " << (double)countTimer / 1000 << " sec" << Qt::endl;

	mw_db.removeDatabase(myParamForSmtp->odbc);

	if (boolSendAfterCreate)
		myParamForSmtp->sendMailfromButton();

	if (boolDelAfterSend && boolSendAfterCreate)
	{
		file.remove(savedFile);
		qDebug() << "\n" << fileName << " was delete\n";
	}

	fileName = "";
}


void DbSqlExport::generalXmlLoop(QString any, QString dayFunc, QString nightFunc, QString counterGuid, QString dateDay)
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

		QString curDate = dateDay;

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
		if (nightFunc == "" && desc == "9")
		{
			night = "0";
			curDate = "189912300200";
		}

		xmlWriter.writeCharacters(curDate);

		xmlWriter.writeEndElement(); // timestamp

		xmlWriter.writeStartElement("value");

		if (internalCounter == 0)
		{
			day[day.indexOf('.')] = ',';

			if ((day.length() - day.indexOf(',')) > 4)
				day.chop(day.length() - (day.indexOf(',') + 5));

			if (day != '0')
			{
				for (int val = (day.length() - day.indexOf(',')); val <= 4; val++)
				{
					day.push_back("0");
				}
			}
			else
				day = "0,0000";

			xmlWriter.writeCharacters(day);
		}

		if (internalCounter == 1)
		{
			night[night.indexOf('.')] = ',';

			if ((night.length() - night.indexOf(',')) > 4)
				night.chop(night.length() - (night.indexOf(',') + 5));

			if (night != '0')
			{
				for (int val = (night.length() - night.indexOf(',')); val <= 4; val++)
				{
					night.push_back("0");
				}
			}
			else
				night = "0,0000";

			xmlWriter.writeCharacters(night);
		}
		if (internalCounter == 2) xmlWriter.writeCharacters("0,0000");

		xmlWriter.writeEndElement(); // value

		xmlWriter.writeEndElement(); // period

		xmlWriter.writeEndElement(); // measurechannel

		desc = "0";
	}

	xmlWriter.writeEndElement(); // measurepoint
}


void DbSqlExport::optionsSmtp()
{
	myParamForSmtp->show();
	myParamForSmtp->readDefaultConfig();
}


void DbSqlExport::checkSendAfterCreate(int myState) {

	if (myState == Qt::Checked) {
		boolSendAfterCreate = true;
	}
	else {
		boolSendAfterCreate = false;
	}
}


void DbSqlExport::checkDelAfterSend(int myState) {

	if (myState == Qt::Checked) {
		boolDelAfterSend = true;
	}
	else {
		boolDelAfterSend = false;
	}
}


void DbSqlExport::slotTimerAlarm()
{
	if (ui.autoSender->isChecked()) {
		generateXml();
	}
}


void DbSqlExport::timerUpdate()
{
	if (ui.autoSender->isChecked())
	{
		timer->stop();
		sBar->showMessage("Autocreate was stoped");
	}
	else
	{
		timer->setInterval(myParamForSmtp->timerTime * 3600000);
		timer->start();
		QString curTime = (QTime::currentTime().toString("hh:mm:ss"));
		QString curDate = (QDate::currentDate().toString("dd.MM.yyyy"));
		sBar->showMessage("Autocreate was start in " + curDate + " " + curTime);
	}
}


void DbSqlExport::MessegeAboutReconnectDb(QString)
{
	connectDataBase();
}


void DbSqlExport::import80020()
{
	bufferFor80020Import.clear();

	/*
	if (myParamForSmtp->odbc != QString("DBEN") || myParamForSmtp->odbc != QString("DBEG") || myParamForSmtp->odbc != QString("DBEY"))
	{
		sBar->showMessage("Wrong DataBase. Please connect for correct DB.");
		return;
	}
	*/
	QString addFileDonor = QFileDialog::getOpenFileName(0, "Add list of numbers", "", "*.xls *.xlsx");

	if (addFileDonor == "")
	{
		return;
	}

	QAxObject* excelDonor = new QAxObject("Excel.Application", 0);
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

	QAxObject* sheetDonor = sheetsDonor->querySubObject("Item(int)", listDonor);// Тут определяем лист с которым будем работать

	QAxObject* usedRangeDonor = sheetDonor->querySubObject("UsedRange"); // так можем получить количество строк в документе
	QAxObject* rowsDonor = usedRangeDonor->querySubObject("Rows");
	int countRowsDonor = rowsDonor->property("Count").toInt();

	QAxObject* usedRangeColDonor = sheetDonor->querySubObject("UsedRange"); // так можем получить количество столбцов в документе
	QAxObject* columnsDonor = usedRangeColDonor->querySubObject("Columns");
	int countColsDonor = columnsDonor->property("Count").toInt();

	QAxObject* cell = nullptr;

	if (!(countColsDonor < 2))
	{

		for (int row = 1; row <= countRowsDonor; ++row)
		{
			cell = sheetDonor->querySubObject("Cells(int,int)", row, 1); // так указываем с какой ячейкой работать
			QString serialString = cell->property("Value").toString().trimmed();

			cell = sheetDonor->querySubObject("Cells(int,int)", row, 2); // так указываем с какой ячейкой работать
			QString iDlString = cell->property("Value").toString().trimmed();

			if (!serialString.isEmpty())
			{
				if (serialString.length() > 18)
				{
					qDebug() << "Incorrect length in row #" << row;
					delete cell;
					cell = nullptr;

					workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
					excelDonor->dynamicCall("Quit()");

					delete workbookDonor;
					delete excelDonor;
					return;
				}

				if (serialString.length() < 5)
				{
					qDebug() << "Incorrect length in row #" << row;
					delete cell;
					cell = nullptr;

					workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
					excelDonor->dynamicCall("Quit()");

					delete workbookDonor;
					delete excelDonor;
					return;
				}

				for (auto& val : serialString)
				{
					if (!val.isDigit())
					{
						qDebug() << "Incorrect number in row #" << row;
						delete cell;
						cell = nullptr;

						workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
						excelDonor->dynamicCall("Quit()");

						delete workbookDonor;
						delete excelDonor;
						return;
					}
				}

				if (iDlString.length() > 40)
				{
					iDlString = "";
					qDebug() << "Incorrect length for ID in row #" << row << ". Id will equal void.";
				}

				bufferFor80020Import.push_back(qMakePair(serialString, iDlString));

				countOfNumbers++;
			}
		}

		qDebug() << "Count of object for import = " << bufferFor80020Import.length();

		importClassBirth();
	}
	else
	{
		qDebug() << "Incorrect format of file. Not find second column.";
	}

	delete cell;
	cell = nullptr;

	workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
	excelDonor->dynamicCall("Quit()");

	delete workbookDonor;
	delete excelDonor;
}


void DbSqlExport::importClassBirth()
{
	QSqlQuery query;
	QString queryString;

	if (myParamForSmtp->odbc == "DBEN" || myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEY")
	{
		queryString = "select count(*) from NDIETable where NodeType = 0"; // Запрашиваем список макетов

		query.exec(queryString);
		query.next();

		int countOfMaket = query.value(0).toInt();

		queryString = "select Name from NDIETable where NodeType = 0"; // Запрашиваем список макетов
		
		query.exec(queryString);
		
		importClass->clearWidget();

		for(int valuesOfQuery = 0; valuesOfQuery < countOfMaket; valuesOfQuery++)
		{
			query.next();
			importClass->setMaket(query.value(0).toString());
		}
		
		importClass->show();
	}
}


void DbSqlExport::processWriteInDb(QString any)
{
	QSqlQuery query;
	QString queryString;

	for (auto& val : bufferFor80020Import)
	{
		queryString = "select ID_MeterInfo from MeterInfo where SN = '" + val.first + "'"; // запрашиваем первичный ID по номеру прибора
		query.exec(queryString);
		query.next();
		QString iD = query.value(0).toString();

		queryString = "select ID_Point from MeterMountHist where ID_MeterInfo = '" + iD + "'"; // получаем ID из счётчика
		query.exec(queryString);
		query.next();
		QString secondiD = query.value(0).toString();

		queryString = "insert into NDIETable(ID_Format, NodeType, Name, Code, ID_Rec, ID_Parent) Values(34, 4, 'Счетчик СПОДЭС-D №" + val.first + "', '" + val.second + "', (select ID_DIE from NDIETable where Name = '" + any + "'), (select ID_DIE from NDIETable where ID_Parent = (select ID_DIE from NDIETable where Name = '" + any + "') and NodeType = 3))";
		query.exec(queryString);

		queryString = "insert into NDIEParams(ID_DIE, ID_DIEPrmType, Value, IntVal) values((select TOP 1 ID_DIE from NDIETable ORDER BY ID_DIE DESC), 61, " + secondiD + ", " + secondiD + ")";
		query.exec(queryString);


		queryString = "select * from dbo.PointParams where ID_Point = '" + secondiD + "' and ID_Param = '4'"; // получаем ID параметра активной энергии счётчика
		query.exec(queryString);
		query.next();
		QString IdParams = query.value(0).toString();

		queryString = "insert into NDIETable(ID_Format, NodeType, Name, Code, ID_Rec, ID_Parent, ID_PP) Values(34, 2, 'Активная энергия, прием', '01', (select ID_DIE from NDIETable where Name = '" + any + "'), (select TOP 1 ID_DIE from NDIETable ORDER BY ID_DIE DESC), " + IdParams + ")";
	}
}
	/*


		

}


//use[ProSoft_ASKUE - Utek]

/*
select ID_Point from MeterMountHist where ID_MeterInfo = (select ID_MeterInfo from MeterInfo where SN = '101000004224')

select ID_PP from dbo.PointParams where ID_Point = (select ID_Point from MeterMountHist where ID_MeterInfo = (select ID_MeterInfo from MeterInfo where SN = '101000004224')) and ID_Param = '4'
*/
//select* from NDIEParams
//select* from NDIETable


/*
insert into NDIETable (ID_Format, NodeType, Name, Code, ID_Rec, ID_Parent)
Values (34, 4, 'Счетчик СПОДЭС-D №101000004224', '9999999999', (select ID_DIE from NDIETable where Name = 'Дополнительный тестовый макет'),
(select ID_DIE from NDIETable where ID_Parent = (select ID_DIE from NDIETable where Name = 'Дополнительный тестовый макет') and NodeType = 3))

insert into NDIEParams(ID_DIE, ID_DIEPrmType, Value, IntVal)
values ((select TOP 1 ID_DIE from NDIETable ORDER BY ID_DIE DESC), 61, (select ID_Point from MeterMountHist where ID_MeterInfo = (select ID_MeterInfo from MeterInfo where SN = '101000004224')),
(select ID_Point from MeterMountHist where ID_MeterInfo = (select ID_MeterInfo from MeterInfo where SN = '101000004224')))

insert into NDIETable (ID_Format, NodeType, Name, Code, ID_Rec, ID_Parent, ID_PP) Values (34, 2, 'Активная энергия, прием', '01', (select ID_DIE from NDIETable where Name = 'Дополнительный тестовый макет'), (select TOP 1 ID_DIE from NDIETable ORDER BY ID_DIE DESC), 1766)
*/

//--select TOP 1 ID_DIE from NDIETable ORDER BY ID_DIE DESC

//--update NDIETable set ID_PP = 1766 where ID_DIE = 21

/*
delete from NDIETable where ID_PP = 1766 OR Code = '9999999999'
delete from NDIEParams where IntVal = '512'
*/


