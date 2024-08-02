#include "ParamSmtp.h"

#include <QElapsedTimer>
#include <QtNetwork/QSslSocket>

#include <QFile>

#include "DbSqlExport.h"

ParamSmtp::ParamSmtp(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//connect(ui.sendBtn_4, SIGNAL(clicked()), this, SLOT(sendMailfromButton()));
	connect(ui.exitBtn_4, SIGNAL(clicked()), this, SLOT(close()));
	//connect(ui.browseBtn_5, SIGNAL(clicked()), this, SLOT(browse()));
	connect(ui.sendBtn_4, SIGNAL(clicked()), this, SLOT(writeCurrent()));

	readDefaultConfig();

}

ParamSmtp::~ParamSmtp()
{}

void ParamSmtp::fileNameSetter(QString any)
{
	fileForSend = any;
}

void ParamSmtp::sendMailfromButton()
{

    SMTP * mySMTP = new SMTP(ui.uname->text(), ui.paswd->text(), ui.server->text(), ui.port->text().toInt());

    connect(mySMTP, SIGNAL(status(QString)), this, SLOT(MessegeAboutMailSend(QString)));

    mySMTP->sendMail(ui.uname->text(), ui.rcpt->text(), ui.subject->text(), fileForSend);
}
/*
void ParamSmtp::browse()
{
    files.clear(); //очистка массива

    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath()); // установить текущий каталог (каталог профиля пользователя C:/Users/Username
    dialog.setFileMode(QFileDialog::ExistingFiles); // определяем количество и тип элементов которые надо выбрать. Одного существующего файла.

    if (dialog.exec()) // фиксируем диалоговое окно.
        files = dialog.selectedFiles(); // возвращаем список строк с путями к выбранным файлам.

    QString fileListString;
    foreach(QString file, files)
        fileListString.append("\"" + QFileInfo(file).fileName() + "\" "); // возвращаем имена выбранных файлов исключая путь в конец строки

    ui.attachment->setText(fileListString);
	
}
*/

/*
void ParamSmtp::MessegeAboutMailSend(QString status)
{
    if (status == "Message sent")
    {
        qDebug() << "\nMail was send.";

        mySMTP = nullptr;
    }
}
*/


void ParamSmtp::readDefaultConfig()
{
	QFile file("config.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Dont fide config file. Add file with parameters.";
		return;
	}

	QTextStream in(&file);

	int countParam = 0;

	// Считываем файл строка за строкой

	while (!in.atEnd()) // метод atEnd() возвращает true, если в потоке больше нет данных для чтения
	{
		QString line = in.readLine(); // метод readLine() считывает одну строку из потока
		++countParam;
		QString temporary;

		for (auto& val : line)
		{
			temporary += val;
		}

		switch (countParam)
		{

		case(1):
		{
			smtpServer = temporary;
			ui.server->setText(smtpServer);
			break;
		}
		case(2):
		{
			smtpPort = temporary;
			ui.port->setText(smtpPort);
			break;
		}
		case(3):
		{
			userName = temporary;
			ui.uname->setText(userName);
			break;
		}
		case(4):
		{
			password = temporary;
			ui.paswd->setText(password);
			break;
		}
		case(5):
		{
			recipantTo = temporary;
			ui.rcpt->setText(recipantTo);
			break;
		}
		case(6):
		{
			subject = temporary;
			ui.subject->setText(subject);
			break;
		}
		}
	}

	file.close();
}

void ParamSmtp::writeCurrent()
{
    QFile file("config.txt");

    // Открываем файл в режиме "Только для записи"
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file); // поток записываемых данных направляем в файл

        // Для записи данных в файл используем оператор <<
        out << ui.server->text() << Qt::endl;
        out << ui.port->text() << Qt::endl;
        out << ui.uname->text() << Qt::endl;
		out << ui.paswd->text() << Qt::endl;
        out << ui.rcpt->text() << Qt::endl;
        out << ui.subject->text() << Qt::endl;

    }
    else
    {
        qWarning("Could not open file");
    }

    file.close();
}