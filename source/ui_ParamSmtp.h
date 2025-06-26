/********************************************************************************
** Form generated from reading UI file 'ParamSmtp.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAMSMTP_H
#define UI_PARAMSMTP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ParamSmtpClass
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *server;
    QLabel *label_2;
    QLineEdit *port;
    QLabel *label_3;
    QLineEdit *uname;
    QLabel *label_4;
    QLineEdit *paswd;
    QLabel *label_5;
    QLineEdit *rcpt;
    QLabel *label_6;
    QLineEdit *subject;
    QWidget *tab_2;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout_2;
    QLabel *label_7;
    QLineEdit *lineEditHostName;
    QLineEdit *lineEditOdbc;
    QLabel *label_8;
    QLabel *label_9;
    QLineEdit *lineEditUserName;
    QLabel *label_10;
    QLineEdit *lineEditPassword;
    QWidget *tab_5;
    QWidget *formLayoutWidget_2;
    QFormLayout *formLayout_3;
    QLabel *label_11;
    QSpinBox *spinBoxHours;
    QLabel *label_12;
    QPushButton *exitBtn_4;
    QPushButton *sendBtn_4;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ParamSmtpClass)
    {
        if (ParamSmtpClass->objectName().isEmpty())
            ParamSmtpClass->setObjectName(QString::fromUtf8("ParamSmtpClass"));
        ParamSmtpClass->resize(392, 413);
        centralWidget = new QWidget(ParamSmtpClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 0, 441, 291));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        layoutWidget = new QWidget(tab);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 20, 331, 471));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
        formLayout->setVerticalSpacing(9);
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        server = new QLineEdit(layoutWidget);
        server->setObjectName(QString::fromUtf8("server"));
        server->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(server->sizePolicy().hasHeightForWidth());
        server->setSizePolicy(sizePolicy);

        formLayout->setWidget(0, QFormLayout::FieldRole, server);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        port = new QLineEdit(layoutWidget);
        port->setObjectName(QString::fromUtf8("port"));

        formLayout->setWidget(1, QFormLayout::FieldRole, port);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        uname = new QLineEdit(layoutWidget);
        uname->setObjectName(QString::fromUtf8("uname"));

        formLayout->setWidget(2, QFormLayout::FieldRole, uname);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        paswd = new QLineEdit(layoutWidget);
        paswd->setObjectName(QString::fromUtf8("paswd"));
        paswd->setEchoMode(QLineEdit::Normal);

        formLayout->setWidget(3, QFormLayout::FieldRole, paswd);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_5);

        rcpt = new QLineEdit(layoutWidget);
        rcpt->setObjectName(QString::fromUtf8("rcpt"));

        formLayout->setWidget(4, QFormLayout::FieldRole, rcpt);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_6);

        subject = new QLineEdit(layoutWidget);
        subject->setObjectName(QString::fromUtf8("subject"));

        formLayout->setWidget(5, QFormLayout::FieldRole, subject);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        formLayoutWidget = new QWidget(tab_2);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(19, 20, 311, 211));
        formLayout_2 = new QFormLayout(formLayoutWidget);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(formLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_7);

        lineEditHostName = new QLineEdit(formLayoutWidget);
        lineEditHostName->setObjectName(QString::fromUtf8("lineEditHostName"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, lineEditHostName);

        lineEditOdbc = new QLineEdit(formLayoutWidget);
        lineEditOdbc->setObjectName(QString::fromUtf8("lineEditOdbc"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, lineEditOdbc);

        label_8 = new QLabel(formLayoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_8);

        label_9 = new QLabel(formLayoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_9);

        lineEditUserName = new QLineEdit(formLayoutWidget);
        lineEditUserName->setObjectName(QString::fromUtf8("lineEditUserName"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, lineEditUserName);

        label_10 = new QLabel(formLayoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_10);

        lineEditPassword = new QLineEdit(formLayoutWidget);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, lineEditPassword);

        tabWidget->addTab(tab_2, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        formLayoutWidget_2 = new QWidget(tab_5);
        formLayoutWidget_2->setObjectName(QString::fromUtf8("formLayoutWidget_2"));
        formLayoutWidget_2->setGeometry(QRect(9, 9, 365, 31));
        formLayout_3 = new QFormLayout(formLayoutWidget_2);
        formLayout_3->setSpacing(6);
        formLayout_3->setContentsMargins(11, 11, 11, 11);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(formLayoutWidget_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_11);

        spinBoxHours = new QSpinBox(formLayoutWidget_2);
        spinBoxHours->setObjectName(QString::fromUtf8("spinBoxHours"));
        spinBoxHours->setMinimum(1);
        spinBoxHours->setMaximum(240);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, spinBoxHours);

        label_12 = new QLabel(tab_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(10, 40, 351, 31));
        tabWidget->addTab(tab_5, QString());
        exitBtn_4 = new QPushButton(centralWidget);
        exitBtn_4->setObjectName(QString::fromUtf8("exitBtn_4"));
        exitBtn_4->setGeometry(QRect(140, 320, 80, 24));
        sendBtn_4 = new QPushButton(centralWidget);
        sendBtn_4->setObjectName(QString::fromUtf8("sendBtn_4"));
        sendBtn_4->setGeometry(QRect(30, 320, 80, 24));
        ParamSmtpClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ParamSmtpClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 392, 21));
        ParamSmtpClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ParamSmtpClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ParamSmtpClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ParamSmtpClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ParamSmtpClass->setStatusBar(statusBar);

        retranslateUi(ParamSmtpClass);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(ParamSmtpClass);
    } // setupUi

    void retranslateUi(QMainWindow *ParamSmtpClass)
    {
        ParamSmtpClass->setWindowTitle(QCoreApplication::translate("ParamSmtpClass", "ParamSmtp", nullptr));
        label->setText(QCoreApplication::translate("ParamSmtpClass", "Smtp-server:", nullptr));
#if QT_CONFIG(tooltip)
        server->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
        server->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
#if QT_CONFIG(accessibility)
        server->setAccessibleName(QString());
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        server->setAccessibleDescription(QString());
#endif // QT_CONFIG(accessibility)
        server->setText(QCoreApplication::translate("ParamSmtpClass", "smtp.gmail.com", nullptr));
        label_2->setText(QCoreApplication::translate("ParamSmtpClass", "Server port:", nullptr));
        port->setText(QString());
        label_3->setText(QCoreApplication::translate("ParamSmtpClass", "Username:", nullptr));
        uname->setText(QString());
        label_4->setText(QCoreApplication::translate("ParamSmtpClass", "Password:", nullptr));
        paswd->setInputMask(QString());
        paswd->setText(QString());
        label_5->setText(QCoreApplication::translate("ParamSmtpClass", "Recipant to:", nullptr));
        rcpt->setText(QString());
        label_6->setText(QCoreApplication::translate("ParamSmtpClass", "Subject:", nullptr));
        subject->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("ParamSmtpClass", "SMTP", nullptr));
        label_7->setText(QCoreApplication::translate("ParamSmtpClass", "Host name/Empty", nullptr));
        label_8->setText(QCoreApplication::translate("ParamSmtpClass", "DB name/ODBC", nullptr));
        label_9->setText(QCoreApplication::translate("ParamSmtpClass", "User name", nullptr));
        label_10->setText(QCoreApplication::translate("ParamSmtpClass", "Password", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("ParamSmtpClass", "SQL", nullptr));
        label_11->setText(QCoreApplication::translate("ParamSmtpClass", "Hours", nullptr));
        label_12->setText(QCoreApplication::translate("ParamSmtpClass", "How often do you want to generate XML?", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QCoreApplication::translate("ParamSmtpClass", "Autosender", nullptr));
        exitBtn_4->setText(QCoreApplication::translate("ParamSmtpClass", "Exit", nullptr));
        sendBtn_4->setText(QCoreApplication::translate("ParamSmtpClass", "Save all", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ParamSmtpClass: public Ui_ParamSmtpClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMSMTP_H
