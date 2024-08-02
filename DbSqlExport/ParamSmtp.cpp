#include "ParamSmtp.h"

#include <QElapsedTimer>
#include <QtNetwork/QSslSocket>

ParamSmtp::ParamSmtp(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.sendBtn_4, SIGNAL(clicked()), this, SLOT(sendMailfromButton()));
	connect(ui.exitBtn_4, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.browseBtn_5, SIGNAL(clicked()), this, SLOT(browse()));
}

ParamSmtp::~ParamSmtp()
{}


void ParamSmtp::sendMailfromButton()
{

    SMTP * mySMTP = new SMTP(ui.uname->text(), ui.paswd->text(), ui.server->text(), ui.port->text().toInt());

    connect(mySMTP, SIGNAL(status(QString)), this, SLOT(MessegeAboutMailSend(QString)));

    if (!files.isEmpty())
        mySMTP->sendMail(ui.uname->text(), ui.rcpt->text(), ui.subject->text(), ui.msg->toPlainText(), files);
    else
        mySMTP->sendMail(ui.uname->text(), ui.rcpt->text(), ui.subject->text(), ui.msg->toPlainText());

}

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

void ParamSmtp::MessegeAboutMailSend(QString status)
{
    if (status == "Message sent")
    {
        qDebug() << "\nMail was send.";

        mySMTP = nullptr;
    }
}
