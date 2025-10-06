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
#include <QtConcurrent>

QTextStream out(stdout);
QTextStream in(stdin);

DbSqlExport::DbSqlExport(QWidget* parent)
	: QMainWindow(parent), importClass(new Import80020CLass()), importTreeCLass(new importTreeObjectClass())
{
	ui.setupUi(this);

	myParamForSmtp = new ParamSmtp();

	ui.generalProgressBar->hide();
	ui.generalProgressBar->setValue(0);

	connect(ui.pushButtonAddNumber, &QPushButton::clicked, this, &DbSqlExport::addOneNumber);
	connect(ui.pushButtonDeleteNumber, &QPushButton::clicked, this, &DbSqlExport::removeNumber);
	connect(ui.pushButtonDeleteAll, &QPushButton::clicked, this, &DbSqlExport::clearAllNumbers);
	connect(ui.pushButtonAddFrom, &QPushButton::clicked, this, &DbSqlExport::addSomeNumbers);
	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));

	connect(ui.pushButtonGenXml, &QPushButton::clicked, this, &DbSqlExport::startGenerateWithQCouncurent);

	connect(ui.pushButtonSendFiles, &QPushButton::clicked, this, &DbSqlExport::optionsSmtp);

	connect(ui.checkBoxSendAfterCreate, &QCheckBox::stateChanged, this, &DbSqlExport::checkSendAfterCreate);
	connect(ui.checkBoxDelAfterSend, &QCheckBox::stateChanged, this, &DbSqlExport::checkDelAfterSend);

	connect(ui.autoSender, SIGNAL(pressed()), this, SLOT(timerUpdate()));

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
	timer->start(myParamForSmtp->timerTime * 3600000); // Каждые три секунды

	connect(myParamForSmtp, SIGNAL(status(QString)), this, SLOT(MessegeAboutReconnectDb(QString))); // делаем реконект к БД после каждого сохранения настроек.

	importMenu = new QMenu(ui.importDbButton);

	importMenu->addAction("&80020*", [this]() {

		importBool80020 = true;
		import80020();

		});

	importMenu->addAction("&Tree Objects", [this]() {

		importBool80020 = false;
		import80020();

		});

	ui.importDbButton->setMenu(importMenu);

	connect(importClass, SIGNAL(status(QString)), this, SLOT(processWriteInDb(QString))); // делаем реконект к БД после каждого сохранения настроек.
	connect(importTreeCLass, SIGNAL(status(QString, QString)), this, SLOT(processWriteInDbTreeObject(QString, QString))); // делаем реконект к БД после каждого сохранения настроек.
	connect(importTreeCLass, SIGNAL(importReferenceSignal(QString, QString)), this, SLOT(processWriteReferenceInDb(QString, QString))); // делаем реконект к БД после каждого сохранения настроек.

	sBar = new QStatusBar();
	QMainWindow::setStatusBar(sBar);

	dbLabel = new QLabel();
	sBar->addPermanentWidget(dbLabel);
	dbLabel->setStyleSheet("color: rgb(255, 0, 0)");

	dbLabelOdbcOrIp = new QLabel();
	sBar->addPermanentWidget(dbLabelOdbcOrIp);
	dbLabelOdbcOrIp->setStyleSheet("color: rgb(0, 0, 0)");

	sBar->setStyleSheet("QStatusBar::item {border: None;}");

	connectDataBase();

	connect(this, &DbSqlExport::statusBarSignal, this, &DbSqlExport::statusBarRefreshAfterSignal);
	connect(this, &DbSqlExport::buttonEnable, this, &DbSqlExport::setEnableAllButton);
	connect(this, &DbSqlExport::addValueInGeneralProgressBar, this, &DbSqlExport::increeseGeneralProgressBar);
	connect(this, &DbSqlExport::refreshGenProgressBar, this, &DbSqlExport::ClearAndHideGeneralProgressBar);
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
	QSharedPointer<QAxObject>excelDonor(new QAxObject("Excel.Application", 0));
	QSharedPointer<QAxObject>workbooksDonor(excelDonor->querySubObject("Workbooks"));
	QSharedPointer<QAxObject>workbookDonor(workbooksDonor->querySubObject("Open(const QString&)", addFileDonor));
	QSharedPointer<QAxObject>sheetsDonor(workbookDonor->querySubObject("Worksheets"));

	int listDonor = sheetsDonor->property("Count").toInt(); // так можем получить количество листов в документе

	if (listDonor > 1)
	{
		do
		{
			listDonor = QInputDialog::getInt(this, "List nomber", "Whats list do you need?");

			if (!listDonor)
			{
				workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
				excelDonor->dynamicCall("Quit()");
				return;
			}

		} while (listDonor <= 0 || (listDonor > (sheetsDonor->property("Count").toInt())));

	}

	QSharedPointer<QAxObject>sheetDonor(sheetsDonor->querySubObject("Item(int)", listDonor));
	QSharedPointer<QAxObject>usedRangeDonor(sheetDonor->querySubObject("UsedRange"));
	QSharedPointer<QAxObject>rowsDonor(usedRangeDonor->querySubObject("Rows"));
	int countRowsDonor = rowsDonor->property("Count").toInt();
	QSharedPointer<QAxObject>usedRangeColDonor(sheetDonor->querySubObject("UsedRange"));
	QSharedPointer<QAxObject>columnsDonor(usedRangeColDonor->querySubObject("Columns"));
	int countColsDonor = columnsDonor->property("Count").toInt();

	for (int row = 1; row <= countRowsDonor; ++row)
	{
		QSharedPointer<QAxObject>cell(sheetDonor.data()->querySubObject("Cells(int,int)", row, 1)); // так указываем с какой ячейкой работать
		QString currentString = cell->property("Value").toString().trimmed();

		if (!currentString.isEmpty())
		{
			ui.listWidget->addItem(currentString);
			int r = ui.listWidget->count() - 1;
			ui.listWidget->setCurrentRow(r); //наконец, выделяем текущий добавленный элемент с помощью метода setCurrentRow().
			countOfNumbers++;
		}
	}

	workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
	excelDonor->dynamicCall("Quit()");
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
		dbLabel->setStyleSheet("color: rgb(27, 194, 22)");

		dbLabelOdbcOrIp->setText("(" + myParamForSmtp->odbc + ")");
	}

	emit statusBarSignal("");
}


void DbSqlExport::queryDbResult(QString any)
{
	QSqlQuery query;
	QString queryString;

	int iD = 0;

	int guidId;

	if (myParamForSmtp->odbc == "DBZS" || myParamForSmtp->odbc == "DBZM")
	{
		query.prepare(R"(
        select IDOBJECT_PARENT 
        from dbo.PROPERTIES 
        where PROPERTY_VALUE = :pointName and IDTYPE_PROPERTY = '987' ORDER BY IDOBJECT_PARENT DESC)");
		query.bindValue(":pointName", any);

		if (!query.exec() || !query.next()) {
			day = night = dateDay = guid = "";
			return;
		}

		iD = query.value(0).toInt();
		guidId = iD;

		query.prepare(R"(select IDOBJECT_TO from dbo.LINK_OBJECTS where IDOBJECT_FROM = :idObjectFrom and IDTYPE_OBJECT_LINK = '1000011')");
		query.bindValue(":idObjectFrom", iD);

		query.exec();
		query.next();

		if (!query.isNull(0))
		{
			iD = query.value(0).toInt();
		}
		else
		{
			query.prepare(R"(select IDOBJECT_TO from dbo.LINK_OBJECTS where IDOBJECT_FROM = :idObjectFrom ORDER BY IDLINK_OBJECTS DESC)");
			query.bindValue(":idObjectFrom", iD);
			query.exec();
			query.next();
			iD = query.value(0).toInt();
		}

		// Дневные показания
		query.prepare(R"(
        select TOP 1 VALUE_METERING, FORMAT(DATEADD(DAY, 1 ,TIME_END), 'yyyy.MM.dd') as TIME_END 
        from dbo.METERINGS 
        where  IDOBJECT = :idObject AND IDTYPE_OBJECT = '1201001' AND IDOBJECT_AGGREGATE = '1'  AND VALUE_METERING != '0' order by TIME_END DESC)");
		query.bindValue(":idObject", iD);

		if (!query.exec() || !query.next())
		{
		}
		else
			day = query.value(0).toString();

		if (day.length() >= 14) //исправляем ошибку переноса из БД в строку при которой добавляется куча значений после запятой
			day.chop(9);


		// Ночные показания
		dateDay = query.value(1).toString();
		query.prepare(R"(
        select TOP 1 VALUE_METERING, FORMAT(DATEADD(DAY, 1 ,TIME_END), 'yyyy.MM.dd') as TIME_END 
        from dbo.METERINGS 
        where IDOBJECT = :idObject AND IDTYPE_OBJECT = '1202001' AND IDOBJECT_AGGREGATE = '1'  AND VALUE_METERING != '0' order by TIME_END DESC)");
		query.bindValue(":idObject", iD);

		if (!query.exec() || !query.next())
		{
		}
		else
			night = query.value(0).toString();

		if (night.length() >= 14)
			night.chop(9);

		query.prepare(R"(
    SELECT pr.PROPERTY_VALUE
    FROM PROPERTIES AS pr
    JOIN LINK_OBJECTS AS li 
      ON li.IDOBJECT_FROM = pr.IDOBJECT_PARENT 
      AND li.IDTYPE_OBJECT_LINK = '1000011'
    WHERE pr.IDTYPE_PROPERTY = '939' 
      AND li.IDOBJECT_TO = :guidId
    )");

		query.bindValue(":guidId", guidId);

		if (!query.exec() || !query.next()) {
			guid = "";
			return;
		}

		guid = query.value(0).toString();
	}

	if (myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEN" || myParamForSmtp->odbc == "DBEY")
	{
		// Подготовим параметр like и ID_Param
		QString pointNameParam = "%" + any + "%";

		// Первый запрос: получение ID_Point и ID_PP одновременно через JOIN
		query.prepare(R"(
        SELECT p.ID_Point, pp.ID_PP
        FROM Points p
        JOIN PointParams pp ON pp.ID_Point = p.ID_Point AND pp.ID_Param = '4'
        WHERE p.PointName LIKE :pointName AND p.Point_Type = '21'
        )");

		query.bindValue(":pointName", pointNameParam);

		if (!query.exec() || !query.next()) {
			day = night = dateDay = guid = "";
			return;
		}

		int idPP = query.value("ID_PP").toInt();

		// Функция для получения Val и DT из PointRatedNIs по N_Rate
		auto getValAndDate = [&](int n_rate) -> QPair<QString, QString> {   // испоьзуем trailing return type.

			QString qStr;

			if (myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEY") {
				qStr = QString(
					"SELECT TOP(1) Val, FORMAT(DT+1, 'yyyy.MM.dd') AS DT "
					"FROM dbo.PointRatedNIs "
					"WHERE ID_PP = :id_pp AND N_Rate = :n_rate "
					"ORDER BY DT DESC");
			}
			else // DBEN
			{
				qStr = QString(
					"SELECT TOP(1) Val, FORMAT(DT, 'yyyy.MM.dd') AS DT "
					"FROM dbo.PointRatedNIs "
					"WHERE ID_PP = :id_pp AND N_Rate = :n_rate "
					"ORDER BY DT DESC");
			}

			QSqlQuery q;
			q.prepare(qStr);
			q.bindValue(":id_pp", idPP);
			q.bindValue(":n_rate", n_rate);

			if (!q.exec() || !q.next())
				return qMakePair(QString(), QString()); // возвращаем пустые строки

			QString val = q.value(0).toString();
			QString dt = q.value(1).toString();

			if (val.length() >= 14)
				val.chop(9);

			return qMakePair(val, dt);
			};

		// Получаем день и дату дня используя функцию 
		auto dayRes = getValAndDate(1);
		day = dayRes.first;
		dateDay = dayRes.second;

		// Получаем ночь используя функцию 
		auto nightRes = getValAndDate(2);
		night = nightRes.first;

		// Получаем ID_Parent из NDIETable
		query.prepare(
			"SELECT TOP(1) ID_Parent "
			"FROM NDIETable "
			"WHERE ID_PP = :id_pp AND ID_Format = '34' "
			"ORDER BY ID_Parent DESC");

		query.bindValue(":id_pp", idPP);

		if (!query.exec() || !query.next()) {
			guid = "";
			return;
		}

		int idParent = query.value(0).toInt();

		// Получаем GUID по ID_DIE
		query.prepare("SELECT Code FROM NDIETable WHERE ID_DIE = :id_die");
		query.bindValue(":id_die", idParent);

		if (!query.exec() || !query.next()) {
			guid = "";
			return;
		}

		guid = query.value(0).toString();
	}

	if (myParamForSmtp->odbc == "DBKV")
	{
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


void DbSqlExport::generateXml(QStringList any)
{
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

	if (savedFile == "")
	{
		emit buttonEnable();
		emit refreshGenProgressBar();
		return;
	}

	qDebug() << "\nTotal devices in the list: " << countOfNumbers;

	qDebug() << "Wait...";

	QElapsedTimer timer;
	int countTimer = 0; // для итогового вывода времени потраченного на выполнение
	int countDoingIterationForTime = 0; // считаем количество выполнений
	int valueForTimer = 5000; // временной отрезок для подсчёта количества выполнений
	timer.start();

	QFile file(savedFile);

	myParamForSmtp->fileNameSetter(savedFile);

	file.open(QIODevice::WriteOnly);

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

		queryDbResult(any[i]);
		generalXmlLoop(any[i], day, night, guid, dateDay);

		if (valueForTimer - timer.elapsed() <= 100) // для отслеживания количества выполнений каждые 5 секунд.
		{
			valueForTimer += 5000;

			QTime ct = QTime::currentTime(); // возвращаем текущее время

			qDebug() << ct.toString() << "   " << countDoingIterationForTime;

			emit statusBarSignal(ct.toString() + "   " + QString::number(countDoingIterationForTime));

			countDoingIterationForTime = 0;
		}

		emit addValueInGeneralProgressBar();
		QCoreApplication::processEvents();
	}

	xmlWriter.writeEndElement(); // area

	xmlWriter.writeEndElement(); // message

	xmlWriter.writeEndDocument();

	file.close();

	countTimer = timer.elapsed();

	out << "XML was made for = " << (double)countTimer / 1000 << " sec" << Qt::endl;

	emit statusBarSignal("XML was made for = " + QString::number((double)countTimer / 1000) + " sec");

	mw_db.removeDatabase(myParamForSmtp->odbc);

	if (boolSendAfterCreate)
		myParamForSmtp->sendMailfromButton();

	if (boolDelAfterSend && boolSendAfterCreate)
	{
		file.remove(savedFile);
		qDebug() << "\n" << fileName << " was delete\n";
	}

	fileName = "";

	emit buttonEnable();
	emit refreshGenProgressBar();
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

			if (day.indexOf(',') != -1)
			{
				if ((day.length() - day.indexOf(',')) > 4)
					day.chop(day.length() - (day.indexOf(',') + 5));
			}

			if (day != '0')
			{
				if (day.indexOf(',') != -1)
				{
					for (int val = (day.length() - day.indexOf(',')); val <= 4; val++)
					{
						day.push_back("0");
					}
				}
			}
			else
				day = "0,0000";

			xmlWriter.writeCharacters(day);
		}

		if (internalCounter == 1)
		{
			night[night.indexOf('.')] = ',';

			if (night.indexOf(',') != -1)
			{
				if ((night.length() - night.indexOf(',')) > 4)
					night.chop(night.length() - (night.indexOf(',') + 5));
			}

			if (night != '0')
			{
				if (night.indexOf(',') != -1)
				{
					for (int val = (night.length() - night.indexOf(',')); val <= 4; val++)
					{
						night.push_back("0");
					}
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
		startGenerateWithQCouncurent();
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
	countOfNumbersForImport = 0;
	bufferForSerialIdOrGuid.clear();
	bufferHouseStreet.clear();

	if (!dbconnect)
	{
		sBar->showMessage("Need connect to DB.");
		return;
	}

	if (myParamForSmtp->odbc == QString("DBZS") || myParamForSmtp->odbc == QString("DBZM") || myParamForSmtp->odbc == QString("DBKV"))
	{
		sBar->showMessage("Wrong DataBase for this function. Please connect for correct DB.");
		return;
	}

	if (myParamForSmtp->userNameDb != QString("solexpimp"))
	{
		sBar->showMessage("Wrong User for this function. Please connect for correct DB.");
		return;
	}

	QString addFileDonor = QFileDialog::getOpenFileName(0, "Add list of numbers", "", "*.xls *.xlsx");

	if (addFileDonor == "")
	{
		return;
	}

	QSharedPointer<QAxObject>excelDonor(new QAxObject("Excel.Application", 0));
	QSharedPointer<QAxObject>workbooksDonor(excelDonor->querySubObject("Workbooks"));
	QSharedPointer<QAxObject>workbookDonor(workbooksDonor->querySubObject("Open(const QString&)", addFileDonor));
	QSharedPointer<QAxObject>sheetsDonor(workbookDonor->querySubObject("Worksheets"));

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

	QSharedPointer<QAxObject>sheetDonor(sheetsDonor->querySubObject("Item(int)", listDonor));
	QSharedPointer<QAxObject>usedRangeDonor(sheetDonor->querySubObject("UsedRange"));
	QSharedPointer<QAxObject>rowsDonor(usedRangeDonor->querySubObject("Rows"));
	int countRowsDonor = rowsDonor->property("Count").toInt();
	QSharedPointer<QAxObject>usedRangeColDonor(sheetDonor->querySubObject("UsedRange"));
	QSharedPointer<QAxObject>columnsDonor(usedRangeColDonor->querySubObject("Columns"));
	int countColsDonor = columnsDonor->property("Count").toInt();

	if (!(countColsDonor < 2))
	{

		for (int row = 1; row <= countRowsDonor; ++row)
		{
			QSharedPointer<QAxObject>cell(sheetDonor.data()->querySubObject("Cells(int,int)", row, 1)); // так указываем с какой ячейкой работать
			QString serialString = cell->property("Value").toString().trimmed();

			cell.reset(sheetDonor->querySubObject("Cells(int,int)", row, 2)); // так указываем с какой ячейкой работать
			QString iDlString = cell->property("Value").toString().trimmed();

			if (!serialString.isEmpty())
			{
				if (serialString.length() > 18)
				{
					qDebug() << "Incorrect length in row #" << row;
					sBar->showMessage("Incorrect length in row #" + QString::number(row), 5000);

					workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
					excelDonor->dynamicCall("Quit()");
					return;
				}

				if (serialString.length() < 5)
				{
					qDebug() << "Incorrect length in row #" << row;
					sBar->showMessage("Incorrect length in row #" + QString::number(row), 5000);

					workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
					excelDonor->dynamicCall("Quit()");
					return;
				}

				for (auto& val : serialString)
				{
					if (!val.isDigit())
					{
						qDebug() << "Incorrect number in row #" << row;
						sBar->showMessage("Incorrect number in row #" + QString::number(row), 5000);

						workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
						excelDonor->dynamicCall("Quit()");
						return;
					}
				}

				if (iDlString.length() > 40)
				{
					iDlString = "";
					qDebug() << "Incorrect length for ID in row #" << row << ". Id will equal void.";
					sBar->showMessage("Incorrect length for ID in row #" + QString::number(row) + ". Id will equal void.", 5000);
				}

				bufferForSerialIdOrGuid.push_back(qMakePair(serialString, iDlString));

				countOfNumbersForImport++;

				if (countColsDonor > 2)
				{
					cell.reset(sheetDonor->querySubObject("Cells(int,int)", row, 3)); // так указываем с какой ячейкой работать
					QString house = cell->property("Value").toString().trimmed();

					cell.reset(sheetDonor->querySubObject("Cells(int,int)", row, 4)); // так указываем с какой ячейкой работать
					QString street = cell->property("Value").toString().trimmed();

					bufferHouseStreet.push_back(qMakePair(house, street));
				}
			}
		}

		qDebug() << "Count of object for import = " << bufferForSerialIdOrGuid.length();

		sBar->showMessage("Count of object for import = " + QString::number(bufferForSerialIdOrGuid.length()), 5000);

		if (importBool80020)
			importClassBirth80020();
		else
			importTreeObjectBirth();
	}
	else
	{
		qDebug() << "Incorrect format of file. Not find second column.";
		sBar->showMessage("Incorrect format of file. Not find second column.", 5000);
	}

	workbookDonor->dynamicCall("Close()"); // обязательно используем в работе с Excel иначе документы будет фbоном открыт в системе
	excelDonor->dynamicCall("Quit()");
}


void DbSqlExport::importClassBirth80020()
{
	QSqlQuery query;
	QString queryString;

	if (myParamForSmtp->odbc == "DBEN" || myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEY")
	{
		queryString = "select count(*) from NDIETable where NodeType = 0 and ID_Format = 34"; // Запрашиваем список макетов

		query.exec(queryString);
		query.next();

		int countOfMaket = query.value(0).toInt();

		queryString = "select Name from NDIETable where NodeType = 0 and ID_Format = 34"; // Запрашиваем список макетов

		query.exec(queryString);

		importClass->clearWidget();

		for (int valuesOfQuery = 0; valuesOfQuery < countOfMaket; valuesOfQuery++)
		{
			query.next();
			importClass->setMaket(query.value(0).toString());
		}

		importClass->clearTextEdit();
		importClass->show();
		importClass->setCurRow();
	}
}


void DbSqlExport::processWriteInDb(QString any)
{
	importClass->clearTextEdit();
	QString errorQuery = "";

	QSqlQuery query;
	QString queryString;

	QPointer<QProgressBar> temporaryProgressBarPtr(importClass->getPtrProgressBar());

	int valueProgressBar = 1;

	importClass->printMessage("Count of import object was = " + QString::number(bufferForSerialIdOrGuid.length()));


	for (auto& val : bufferForSerialIdOrGuid)
	{
		queryString = "select ID_MeterInfo from MeterInfo where SN = '" + val.first + "'"; // запрашиваем первичный ID по номеру прибора
		query.exec(queryString);
		query.next();
		if (query.isNull(0)) // запись согласно запросу не найдена
		{
			errorQuery += val.first + "\n";
		}
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
		query.exec(queryString);

		temporaryProgressBarPtr->setValue(valueProgressBar);
		valueProgressBar++;
	}

	temporaryProgressBarPtr->hide();

	importClass->printMessage("Device not found in DataBase: \n" + errorQuery);
}


void DbSqlExport::importTreeObjectBirth()
{
	QSqlQuery query;
	QString queryString;
	int parentID;
	QTreeWidget* temporaryTreeWidgetPtr = importTreeCLass->returnWidget();

	if (myParamForSmtp->odbc == "DBEN" || myParamForSmtp->odbc == "DBEG" || myParamForSmtp->odbc == "DBEY")
	{
		QTreeWidget* temporaryTreeWidgetPtr = importTreeCLass->returnWidget();

		temporaryTreeWidgetPtr->clear();

		queryString = "SELECT count(*) FROM Points where Point_Type != '21' and Point_Type != '145' and Point_Type != '10' and Point_Type != '81' and Point_Type != '43' and Point_Type != '255' and Point_Type != '49'"; // Запрашиваем список макетов

		query.exec(queryString);
		query.next();

		int countOfMaket = query.value(0).toInt();

		qDebug() << countOfMaket;

		queryString = "SELECT ID_Point, PointName, ID_Parent, Point_Type FROM Points where Point_Type != '21' and Point_Type != '145' and Point_Type != '10' and Point_Type != '81'  and Point_Type != '43' and Point_Type != '255' and Point_Type != '49' order by ID_Parent"; // Запрашиваем список макетов

		query.exec(queryString);

		ui.generalProgressBar->setMaximum(countOfMaket);

		ui.generalProgressBar->show();

		sBar->showMessage("Count of object for import = " + QString::number(bufferForSerialIdOrGuid.length()) + ". Tree object = " + QString::number(countOfMaket));

		for (int valuesOfQuery = 0; valuesOfQuery < countOfMaket; valuesOfQuery++)
		{
			ui.generalProgressBar->setValue(valuesOfQuery);

			query.next();

			parentID = query.value(2).toInt();

			QTreeWidgetItem* any = nullptr;

			if (parentID == 0)
			{
				any = new QTreeWidgetItem();
				temporaryTreeWidgetPtr->addTopLevelItem(any);
			}
			else
			{
				any = new QTreeWidgetItem();

				QList <QTreeWidgetItem*> myList = temporaryTreeWidgetPtr->findItems(QString::number(parentID), Qt::MatchRecursive, 1);

				for (auto& val : myList)
				{
					val->addChild(any);
				}
			}

			any->setText(0, query.value(1).toString());
			any->setText(1, query.value(0).toString());
			any->setText(2, query.value(3).toString());

			if (any->text(2) == "144")
			{
				any->setBackground(0, QColor(120, 228, 171, 255));
			}

			if (any->text(2) == "143")
			{
				any->setBackground(0, QColor(243, 255, 63, 255));
			}

			if (any->text(2) == "141")
			{
				any->setBackground(0, QColor(241, 116, 116, 255));
			}
		}

		QList <QTreeWidgetItem*> myList = temporaryTreeWidgetPtr->findItems("Новые счетчики", Qt::MatchRecursive, 0);

		for (auto& val : myList)
		{
			delete val;
		}

		QList <QTreeWidgetItem*> myListSecond = temporaryTreeWidgetPtr->findItems("Ведро (для мусора)", Qt::MatchRecursive, 0);

		for (auto& val : myListSecond)
		{
			delete val;
		}
	}

	for (int countOfTop = 0; countOfTop < temporaryTreeWidgetPtr->topLevelItemCount(); countOfTop++)
	{
		QTreeWidgetItem* some = temporaryTreeWidgetPtr->topLevelItem(countOfTop);
		recursionSorting(some);
		some = nullptr;
	}

	temporaryTreeWidgetPtr->setCurrentItem(nullptr);

	ui.generalProgressBar->hide();
	importTreeCLass->clearTextEdit();
	importTreeCLass->show();
}


void DbSqlExport::recursionSorting(QTreeWidgetItem* some)
{
	if (some->childCount())
	{
		some->sortChildren(0, Qt::AscendingOrder);

		int count = some->childCount();

		for (int x = 0; x < count; x++)
		{
			recursionSorting(some->child(x));
		}
	}
	else
	{
		return;
	}
}


void DbSqlExport::processWriteInDbTreeObject(QString any, QString idAny)
{
	importTreeCLass->clearTextEdit();

	QSqlQuery query;
	QString queryString;
	QString errorQuery = "";
	int idObjectWasFind;

	QPointer<QProgressBar> temporaryProgressBarPtr(importTreeCLass->getPtrProgressBar());

	temporaryProgressBarPtr->setValue(0);
	temporaryProgressBarPtr->setMaximum(bufferForSerialIdOrGuid.length());
	temporaryProgressBarPtr->show();

	int counterOfIterations = 0;
	int houseId;
	int streetId;

	int createStreet = 0;
	int createHouse = 0;
	int createRoom = 0;

	QString nameTypeObject;

	switch (idAny.toInt())
	{
	case(141): {
		nameTypeObject = "Town";
		break;
	}
	case(143): {
		nameTypeObject = "Street";
		break;
	}
	case(144): {
		nameTypeObject = "House";
		break;
	}
	}

	importTreeCLass->printMessage("Count of import object was = " + QString::number(bufferForSerialIdOrGuid.length()) + ". Object imported into: " + nameTypeObject);

	if (idAny == QString::number(141))
	{
		for (auto& val : bufferHouseStreet)
		{
			queryString = "select ID_Point from Points where PointName = '" + val.second + "' and ID_Parent = " + any;
			query.exec(queryString);
			query.next();

			if (query.isNull(0)) // запись согласно запросу не найдена
			{
				queryString = "insert into Points(PointName, ID_Parent, Point_Type) values('" + val.second + "', " + any + ", 143)";
				query.exec(queryString);
				queryString = "select ID_Point from Points where PointName = '" + val.second + "' and ID_Parent = " + any;
				query.exec(queryString);
				query.next();
				streetId = query.value(0).toInt();
				createStreet++;
			}
			else
			{
				streetId = query.value(0).toInt();
			}

			queryString = "select ID_Point from Points where PointName = '" + val.first + "' and ID_Parent = " + QString::number(streetId);
			query.exec(queryString);
			query.next();

			if (query.isNull(0)) // запись согласно запросу не найдена
			{
				queryString = "insert into Points(PointName, ID_Parent, Point_Type) values('" + val.first + "', " + QString::number(streetId) + ", 144)";
				query.exec(queryString);
				queryString = "select ID_Point from Points where PointName = '" + val.first + "' and ID_Parent = " + QString::number(streetId);
				query.exec(queryString);
				query.next();
				houseId = query.value(0).toInt();
				createHouse++;
			}
			else
			{
				houseId = query.value(0).toInt();
			}

			queryString = "select ID_Point from Points where PointName = '" + bufferForSerialIdOrGuid[counterOfIterations].second + "' and ID_Parent = " + QString::number(houseId);
			query.exec(queryString);
			query.next();

			if (!query.isNull(0)) // запись согласно запросу найдена
			{
				idObjectWasFind = query.value(0).toInt();
				queryString = "update Points set ID_Parent = " + QString::number(idObjectWasFind) + " where PointName like '%" + bufferForSerialIdOrGuid[counterOfIterations].first + "%'";
				query.exec(queryString);

				if (query.numRowsAffected() == 0)
				{
					errorQuery += bufferForSerialIdOrGuid[counterOfIterations].first + "\n";
				}
			}
			else
			{
				queryString = "insert into Points(PointName, ID_Parent, Point_Type) values('" + bufferForSerialIdOrGuid[counterOfIterations].second + "', " + QString::number(houseId) + ", 145)";
				query.exec(queryString);
				createRoom++;

				queryString = "update Points set ID_Parent = (SELECT TOP (1) ID_Point FROM Points  order by ID_Point DESC) where PointName like '%" + bufferForSerialIdOrGuid[counterOfIterations].first + "%'";
				query.exec(queryString);

				if (query.numRowsAffected() == 0)
				{
					errorQuery += bufferForSerialIdOrGuid[counterOfIterations].first + "\n";
				}
			}

			if (importTreeCLass->getPtrCheckBoxTariff().data()->isChecked())
			{
				queryString = "insert into Point_TariffCategory values(1, (SELECT ID_Point FROM Points  where PointName like '%" + bufferForSerialIdOrGuid[counterOfIterations].first + "'), '1990 - 01 - 01 00:00 : 00.000', '2050 - 01 - 01 00 : 00 : 00.000')";
				query.exec(queryString);
			}

			counterOfIterations++;

			temporaryProgressBarPtr->setValue(temporaryProgressBarPtr->value() + 1);
		}
	}

	if (idAny == QString::number(143))
	{
		for (auto& val : bufferHouseStreet)
		{
			queryString = "select ID_Point from Points where PointName = '" + val.first + "' and ID_Parent = " + any;
			query.exec(queryString);
			query.next();

			if (query.isNull(0)) // запись согласно запросу не найдена
			{
				queryString = "insert into Points(PointName, ID_Parent, Point_Type) values('" + val.first + "', " + any + ", 144)";
				query.exec(queryString);
				queryString = "select ID_Point from Points where PointName = '" + val.first + "' and ID_Parent = " + any;
				query.exec(queryString);
				query.next();
				houseId = query.value(0).toInt();
				createHouse++;
			}
			else
			{
				houseId = query.value(0).toInt();
			}

			queryString = "select ID_Point from Points where PointName = '" + bufferForSerialIdOrGuid[counterOfIterations].second + "' and ID_Parent = " + QString::number(houseId);
			query.exec(queryString);
			query.next();

			if (!query.isNull(0)) // запись согласно запросу найдена
			{
				idObjectWasFind = query.value(0).toInt();
				queryString = "update Points set ID_Parent = " + QString::number(idObjectWasFind) + " where PointName like '%" + bufferForSerialIdOrGuid[counterOfIterations].first + "%'";
				query.exec(queryString);

				if (query.numRowsAffected() == 0)
				{
					errorQuery += bufferForSerialIdOrGuid[counterOfIterations].first + "\n";
				}
			}
			else
			{
				queryString = "insert into Points(PointName, ID_Parent, Point_Type) values('" + bufferForSerialIdOrGuid[counterOfIterations].second + "', " + QString::number(houseId) + ", 145)";
				query.exec(queryString);

				queryString = "update Points set ID_Parent = (SELECT TOP (1) ID_Point FROM Points  order by ID_Point DESC) where PointName like '%" + bufferForSerialIdOrGuid[counterOfIterations].first + "%'";
				query.exec(queryString);
				createRoom++;

				if (query.numRowsAffected() == 0)
				{
					errorQuery += bufferForSerialIdOrGuid[counterOfIterations].first + "\n";
				}
			}

			if (importTreeCLass->getPtrCheckBoxTariff().data()->isChecked())
			{
				queryString = "insert into Point_TariffCategory values(1, (SELECT ID_Point FROM Points  where PointName like '%" + bufferForSerialIdOrGuid[counterOfIterations].first + "'), '1990 - 01 - 01 00:00 : 00.000', '2050 - 01 - 01 00 : 00 : 00.000')";
				query.exec(queryString);
			}

			counterOfIterations++;

			temporaryProgressBarPtr->setValue(temporaryProgressBarPtr->value() + 1);
		}
	}


	if (idAny == QString::number(144))
	{
		for (auto& val : bufferForSerialIdOrGuid)
		{
			queryString = "select ID_Point from Points where PointName = '" + val.second + "' and ID_Parent = " + any;
			query.exec(queryString);
			query.next();

			if (!query.isNull(0)) // запись согласно запросу найдена
			{
				idObjectWasFind = query.value(0).toInt();
				queryString = "update Points set ID_Parent = " + QString::number(idObjectWasFind) + " where PointName like '%" + val.first + "%'";
				query.exec(queryString);

				if (query.numRowsAffected() == 0)
				{
					errorQuery += val.first + "\n";
				}
			}
			else
			{
				queryString = "insert into Points(PointName, ID_Parent, Point_Type) values('" + val.second + "', " + any + ", 145)";
				query.exec(queryString);

				queryString = "update Points set ID_Parent = (SELECT TOP (1) ID_Point FROM Points  order by ID_Point DESC) where PointName like '%" + val.first + "%'";
				query.exec(queryString);
				createRoom++;

				if (query.numRowsAffected() == 0)
				{
					errorQuery += val.first + "\n";
				}
			}

			if (importTreeCLass->getPtrCheckBoxTariff().data()->isChecked())
			{
				queryString = "insert into Point_TariffCategory values(1, (SELECT ID_Point FROM Points  where PointName like '%" + val.first + "'), '1990 - 01 - 01 00:00 : 00.000', '2050 - 01 - 01 00 : 00 : 00.000')";
				query.exec(queryString);
			}

			temporaryProgressBarPtr->setValue(temporaryProgressBarPtr->value() + 1);
		}
	}

	importTreeCLass->printMessage("Streets was added =  " + QString::number(createStreet) + "\nHouses was added =  " + QString::number(createHouse) + "\nRooms was added =  " + QString::number(createRoom));

	importTreeCLass->printMessage("Device not found in DataBase: \n" + errorQuery);

	temporaryProgressBarPtr.data()->hide();
}


void DbSqlExport::processWriteReferenceInDb(QString any, QString idAny)
{
	importTreeCLass->clearTextEdit();

	QSqlQuery query;
	QString queryString;
	QString errorQuery = "";
	int idObjectWasFind;
	QString pointName = "";

	QPointer<QProgressBar> temporaryProgressBarPtr(importTreeCLass->getPtrProgressBar());

	temporaryProgressBarPtr->setValue(0);
	temporaryProgressBarPtr->setMaximum(bufferForSerialIdOrGuid.length());
	temporaryProgressBarPtr->show();

	int houseId;
	int streetId;

	int createReference = 0;

	QString nameTypeObject;

	switch (idAny.toInt())
	{
	case(141): {
		nameTypeObject = "Town";
		break;
	}
	case(143): {
		nameTypeObject = "Street";
		break;
	}
	case(144): {
		nameTypeObject = "House";
		break;
	}
	}

	importTreeCLass->printMessage("Count of import object was = " + QString::number(bufferForSerialIdOrGuid.length()) + ". Object imported into: " + nameTypeObject);

	for (auto& val : bufferForSerialIdOrGuid)
	{
		queryString = "select ID_Parent from Points where PointName like '%" + val.first + "%'";
		query.exec(queryString);
		query.next();

		if (!query.isNull(0)) // запись согласно запросу найдена
		{
			idObjectWasFind = query.value(0).toInt();

			queryString = "select PointName from Points where ID_Point = '" + QString::number(idObjectWasFind) + "'";
			query.exec(queryString);
			query.next();
			pointName = query.value(0).toString();

			queryString = "insert into Points(PointName, ID_Parent, Point_Type, ID_Ref) values('" + pointName + "', " + any + ", 255, " + QString::number(idObjectWasFind) + ")";;
			query.exec(queryString);

			createReference++;
		}
		else
		{
			errorQuery += val.first + "\n";
		}
		temporaryProgressBarPtr->setValue(temporaryProgressBarPtr->value() + 1);
	}

	importTreeCLass->printMessage("References was added =  " + QString::number(createReference));

	importTreeCLass->printMessage("Device not found in DataBase: \n" + errorQuery);

	temporaryProgressBarPtr.data()->hide();
}


void DbSqlExport::statusBarRefreshAfterSignal(QString any)
{
	sBar->showMessage(any);
}


void DbSqlExport::startGenerateWithQCouncurent()
{
	if (ui.listWidget->count() < 1)
	{
		sBar->showMessage("Nothing devices for import in XML");
		return;
	}

	setDisableAllButton();

	ui.generalProgressBar->setMaximum(ui.listWidget->count());

	ui.generalProgressBar->show();

	if (!dbconnect)
	{
		emit statusBarSignal("Need connect to DB.");
		emit buttonEnable();
		emit refreshGenProgressBar();
		return;
	}

	generateXml(createStringArray());

	/*
	QtConcurrent::run([this]() {

		HRESULT hr = CoInitialize(NULL);

		if (SUCCEEDED(hr)) {

			connectDataBase(); // QSqlDataBase нужно отдельно инициировать в каждом потоке. Не получиться общий использовать.

			if (!dbconnect)
			{
				emit statusBarSignal("Need connect to DB.");
				emit buttonEnable();
				emit refreshGenProgressBar();
				return;
			}

			generateXml(createStringArray());
		}
		else
		{
			sBar->showMessage("Not initialize COM object ");
		}

		CoUninitialize();

		});

		*/
}


void DbSqlExport::setDisableAllButton()
{
	ui.pushButtonAddNumber->setEnabled(false);
	ui.pushButtonDeleteNumber->setEnabled(false);
	ui.pushButtonDeleteAll->setEnabled(false);
	ui.pushButtonAddFrom->setEnabled(false);
	ui.pushButtonClose->setEnabled(false);
	ui.pushButtonGenXml->setEnabled(false);
	ui.pushButtonSendFiles->setEnabled(false);
	ui.checkBoxSendAfterCreate->setEnabled(false);
	ui.checkBoxDelAfterSend->setEnabled(false);
	ui.autoSender->setEnabled(false);
	ui.importDbButton->setEnabled(false);
}

void DbSqlExport::setEnableAllButton()
{
	ui.pushButtonAddNumber->setEnabled(true);
	ui.pushButtonDeleteNumber->setEnabled(true);
	ui.pushButtonDeleteAll->setEnabled(true);
	ui.pushButtonAddFrom->setEnabled(true);
	ui.pushButtonClose->setEnabled(true);
	ui.pushButtonGenXml->setEnabled(true);
	ui.pushButtonSendFiles->setEnabled(true);
	ui.checkBoxSendAfterCreate->setEnabled(true);
	ui.checkBoxDelAfterSend->setEnabled(true);
	ui.autoSender->setEnabled(true);
	ui.importDbButton->setEnabled(true);
}

QStringList DbSqlExport::createStringArray()
{
	QStringList strArr;

	for (int i = 0; i < countOfNumbers; i++)
	{
		ui.listWidget->setCurrentRow(i);

		strArr.append(ui.listWidget->currentItem()->text());
	}

	return strArr;
}

void DbSqlExport::increeseGeneralProgressBar()
{
	ui.generalProgressBar->setValue(ui.generalProgressBar->value() + 1);
}

void DbSqlExport::ClearAndHideGeneralProgressBar()
{
	ui.generalProgressBar->hide();
	ui.generalProgressBar->setValue(0);
}