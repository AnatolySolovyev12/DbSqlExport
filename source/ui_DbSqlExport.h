/********************************************************************************
** Form generated from reading UI file 'DbSqlExport.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBSQLEXPORT_H
#define UI_DBSQLEXPORT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DbSqlExportClass
{
public:
    QAction *action2;
    QAction *action33;
    QAction *action55;
    QAction *action66;
    QWidget *centralWidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButtonClose;
    QSpacerItem *horizontalSpacer_2;
    QListWidget *listWidget;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButtonAddNumber;
    QPushButton *pushButtonDeleteNumber;
    QPushButton *pushButtonDeleteAll;
    QPushButton *pushButtonAddFrom;
    QPushButton *pushButtonGenXml;
    QPushButton *pushButtonSendFiles;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *checkBoxSendAfterCreate;
    QCheckBox *checkBoxDelAfterSend;
    QRadioButton *autoSender;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_2;
    QStatusBar *statusBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *DbSqlExportClass)
    {
        if (DbSqlExportClass->objectName().isEmpty())
            DbSqlExportClass->setObjectName(QString::fromUtf8("DbSqlExportClass"));
        DbSqlExportClass->resize(505, 549);
        action2 = new QAction(DbSqlExportClass);
        action2->setObjectName(QString::fromUtf8("action2"));
        action33 = new QAction(DbSqlExportClass);
        action33->setObjectName(QString::fromUtf8("action33"));
        action55 = new QAction(DbSqlExportClass);
        action55->setObjectName(QString::fromUtf8("action55"));
        action66 = new QAction(DbSqlExportClass);
        action66->setObjectName(QString::fromUtf8("action66"));
        centralWidget = new QWidget(DbSqlExportClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayoutWidget = new QWidget(centralWidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(0, 0, 491, 501));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pushButtonClose = new QPushButton(gridLayoutWidget);
        pushButtonClose->setObjectName(QString::fromUtf8("pushButtonClose"));

        verticalLayout->addWidget(pushButtonClose);


        gridLayout->addLayout(verticalLayout, 2, 5, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 3, 1, 1);

        listWidget = new QListWidget(gridLayoutWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout->addWidget(listWidget, 1, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 3, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pushButtonAddNumber = new QPushButton(gridLayoutWidget);
        pushButtonAddNumber->setObjectName(QString::fromUtf8("pushButtonAddNumber"));

        verticalLayout_2->addWidget(pushButtonAddNumber);

        pushButtonDeleteNumber = new QPushButton(gridLayoutWidget);
        pushButtonDeleteNumber->setObjectName(QString::fromUtf8("pushButtonDeleteNumber"));

        verticalLayout_2->addWidget(pushButtonDeleteNumber);

        pushButtonDeleteAll = new QPushButton(gridLayoutWidget);
        pushButtonDeleteAll->setObjectName(QString::fromUtf8("pushButtonDeleteAll"));

        verticalLayout_2->addWidget(pushButtonDeleteAll);

        pushButtonAddFrom = new QPushButton(gridLayoutWidget);
        pushButtonAddFrom->setObjectName(QString::fromUtf8("pushButtonAddFrom"));

        verticalLayout_2->addWidget(pushButtonAddFrom);

        pushButtonGenXml = new QPushButton(gridLayoutWidget);
        pushButtonGenXml->setObjectName(QString::fromUtf8("pushButtonGenXml"));

        verticalLayout_2->addWidget(pushButtonGenXml);

        pushButtonSendFiles = new QPushButton(gridLayoutWidget);
        pushButtonSendFiles->setObjectName(QString::fromUtf8("pushButtonSendFiles"));

        verticalLayout_2->addWidget(pushButtonSendFiles);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        checkBoxSendAfterCreate = new QCheckBox(gridLayoutWidget);
        checkBoxSendAfterCreate->setObjectName(QString::fromUtf8("checkBoxSendAfterCreate"));

        verticalLayout_3->addWidget(checkBoxSendAfterCreate);

        checkBoxDelAfterSend = new QCheckBox(gridLayoutWidget);
        checkBoxDelAfterSend->setObjectName(QString::fromUtf8("checkBoxDelAfterSend"));

        verticalLayout_3->addWidget(checkBoxDelAfterSend);

        autoSender = new QRadioButton(gridLayoutWidget);
        autoSender->setObjectName(QString::fromUtf8("autoSender"));

        verticalLayout_3->addWidget(autoSender);


        verticalLayout_2->addLayout(verticalLayout_3);


        gridLayout->addLayout(verticalLayout_2, 1, 5, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 4, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 1, 0, 1, 1);

        DbSqlExportClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(DbSqlExportClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        DbSqlExportClass->setStatusBar(statusBar);
        mainToolBar = new QToolBar(DbSqlExportClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        DbSqlExportClass->addToolBar(Qt::BottomToolBarArea, mainToolBar);

        retranslateUi(DbSqlExportClass);

        QMetaObject::connectSlotsByName(DbSqlExportClass);
    } // setupUi

    void retranslateUi(QMainWindow *DbSqlExportClass)
    {
        DbSqlExportClass->setWindowTitle(QCoreApplication::translate("DbSqlExportClass", "DbSqlExport", nullptr));
        action2->setText(QCoreApplication::translate("DbSqlExportClass", "2", nullptr));
        action33->setText(QCoreApplication::translate("DbSqlExportClass", "33", nullptr));
        action55->setText(QCoreApplication::translate("DbSqlExportClass", "55", nullptr));
        action66->setText(QCoreApplication::translate("DbSqlExportClass", "66", nullptr));
        pushButtonClose->setText(QCoreApplication::translate("DbSqlExportClass", "Close", nullptr));
        pushButtonAddNumber->setText(QCoreApplication::translate("DbSqlExportClass", "Add number", nullptr));
        pushButtonDeleteNumber->setText(QCoreApplication::translate("DbSqlExportClass", "Delete number", nullptr));
        pushButtonDeleteAll->setText(QCoreApplication::translate("DbSqlExportClass", "Delete all", nullptr));
        pushButtonAddFrom->setText(QCoreApplication::translate("DbSqlExportClass", "Add's from file", nullptr));
        pushButtonGenXml->setText(QCoreApplication::translate("DbSqlExportClass", "Generate XML", nullptr));
        pushButtonSendFiles->setText(QCoreApplication::translate("DbSqlExportClass", "Options", nullptr));
        checkBoxSendAfterCreate->setText(QCoreApplication::translate("DbSqlExportClass", "Send after create", nullptr));
        checkBoxDelAfterSend->setText(QCoreApplication::translate("DbSqlExportClass", "Delete after send", nullptr));
        autoSender->setText(QCoreApplication::translate("DbSqlExportClass", "Autocreater", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DbSqlExportClass: public Ui_DbSqlExportClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBSQLEXPORT_H
