/********************************************************************************
** Form generated from reading UI file 'syncia.ui'
**
** Created: Tue Sep 18 02:36:45 2012
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYNCIA_H
#define UI_SYNCIA_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionUploadFile;
    QAction *actionOepnUploadDirectory;
    QAction *actionExit;
    QAction *actionEditDonwloadDirectoryPath;
    QAction *actionUploadDirectoryPath;
    QAction *actionFileCheckInterval;
    QAction *actionHowToUse;
    QAction *actionSyncia;
    QAction *actionDownloadDirectory;
    QAction *actionUploadDirectory;
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QTableWidget *tableWidget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuOpenDownloadDirectory;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(320, 320);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(320, 320));
        MainWindow->setMaximumSize(QSize(778, 614));
        actionUploadFile = new QAction(MainWindow);
        actionUploadFile->setObjectName(QString::fromUtf8("actionUploadFile"));
        actionOepnUploadDirectory = new QAction(MainWindow);
        actionOepnUploadDirectory->setObjectName(QString::fromUtf8("actionOepnUploadDirectory"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionEditDonwloadDirectoryPath = new QAction(MainWindow);
        actionEditDonwloadDirectoryPath->setObjectName(QString::fromUtf8("actionEditDonwloadDirectoryPath"));
        actionUploadDirectoryPath = new QAction(MainWindow);
        actionUploadDirectoryPath->setObjectName(QString::fromUtf8("actionUploadDirectoryPath"));
        actionFileCheckInterval = new QAction(MainWindow);
        actionFileCheckInterval->setObjectName(QString::fromUtf8("actionFileCheckInterval"));
        actionHowToUse = new QAction(MainWindow);
        actionHowToUse->setObjectName(QString::fromUtf8("actionHowToUse"));
        actionSyncia = new QAction(MainWindow);
        actionSyncia->setObjectName(QString::fromUtf8("actionSyncia"));
        actionDownloadDirectory = new QAction(MainWindow);
        actionDownloadDirectory->setObjectName(QString::fromUtf8("actionDownloadDirectory"));
        actionUploadDirectory = new QAction(MainWindow);
        actionUploadDirectory->setObjectName(QString::fromUtf8("actionUploadDirectory"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        centralwidget->setMinimumSize(QSize(778, 562));
        centralwidget->setMaximumSize(QSize(778, 562));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 321, 301));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(3, 0, 3, 3);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, -1, -1, -1);
        lineEdit = new QLineEdit(verticalLayoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);

        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);

        tableWidget = new QTableWidget(verticalLayoutWidget);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        verticalLayout->addWidget(tableWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 320, 19));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuOpenDownloadDirectory = new QMenu(menuFile);
        menuOpenDownloadDirectory->setObjectName(QString::fromUtf8("menuOpenDownloadDirectory"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(menuOpenDownloadDirectory->menuAction());
        menuFile->addAction(actionUploadFile);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuOpenDownloadDirectory->addAction(actionDownloadDirectory);
        menuOpenDownloadDirectory->addAction(actionUploadDirectory);
        menuEdit->addAction(actionEditDonwloadDirectoryPath);
        menuEdit->addAction(actionUploadDirectoryPath);
        menuEdit->addAction(actionFileCheckInterval);
        menuHelp->addAction(actionHowToUse);
        menuHelp->addSeparator();
        menuHelp->addAction(actionSyncia);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Syncia", 0, QApplication::UnicodeUTF8));
        actionUploadFile->setText(QApplication::translate("MainWindow", "UploadFile", 0, QApplication::UnicodeUTF8));
        actionOepnUploadDirectory->setText(QApplication::translate("MainWindow", "OepnUploadDirectory", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionEditDonwloadDirectoryPath->setText(QApplication::translate("MainWindow", "DonwloadDirectoryPath", 0, QApplication::UnicodeUTF8));
        actionUploadDirectoryPath->setText(QApplication::translate("MainWindow", "UploadDirectoryPath", 0, QApplication::UnicodeUTF8));
        actionFileCheckInterval->setText(QApplication::translate("MainWindow", "FileCheckInterval", 0, QApplication::UnicodeUTF8));
        actionHowToUse->setText(QApplication::translate("MainWindow", "HowToUse", 0, QApplication::UnicodeUTF8));
        actionSyncia->setText(QApplication::translate("MainWindow", "Version", 0, QApplication::UnicodeUTF8));
        actionDownloadDirectory->setText(QApplication::translate("MainWindow", "DownloadDirectory", 0, QApplication::UnicodeUTF8));
        actionUploadDirectory->setText(QApplication::translate("MainWindow", "UploadDirectory", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "search", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "FileName", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "HashId", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "BirthUniversalTime", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuOpenDownloadDirectory->setTitle(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYNCIA_H
