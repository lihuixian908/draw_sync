/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "FancyButton.h"
#include "src/Fancy/Slider.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *canvasWidget;
    QWidget *widget;
    FancyButton *colorButton;
    FancyButton *eraserButton;
    FancyButton *undoButton;
    FancyButton *clearButton;
    FancyButton *btnLeaderboard;
    fancy::Slider *widthSlider;
    QLineEdit *serverUrlEdit;
    QLineEdit *playerNameEdit;
    FancyButton *btnConnect;
    FancyButton *btnDisconnect;
    QLineEdit *roomNameEdit;
    FancyButton *btnCreateRoom;
    QLabel *roomStatusLabel;
    FancyButton *btnJoinRoom;
    FancyButton *btnGameStart;
    FancyButton *btnRoundStart;
    FancyButton *btnGameEnd;
    QLabel *wordLabel;
    FancyButton *btnSendChat;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 800);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        canvasWidget = new QWidget(centralwidget);
        canvasWidget->setObjectName("canvasWidget");
        canvasWidget->setGeometry(QRect(-61, -51, 760, 900));
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(800, 20, 180, 740));
        colorButton = new FancyButton(widget);
        colorButton->setObjectName("colorButton");
        colorButton->setGeometry(QRect(20, 20, 75, 24));
        eraserButton = new FancyButton(widget);
        eraserButton->setObjectName("eraserButton");
        eraserButton->setGeometry(QRect(20, 60, 75, 24));
        eraserButton->setCheckable(true);
        eraserButton->setChecked(false);
        undoButton = new FancyButton(widget);
        undoButton->setObjectName("undoButton");
        undoButton->setGeometry(QRect(20, 100, 75, 24));
        clearButton = new FancyButton(widget);
        clearButton->setObjectName("clearButton");
        clearButton->setGeometry(QRect(20, 150, 75, 24));
        btnLeaderboard = new FancyButton(widget);
        btnLeaderboard->setObjectName("btnLeaderboard");
        btnLeaderboard->setGeometry(QRect(105, 20, 65, 24));
        widthSlider = new fancy::Slider(widget);
        widthSlider->setObjectName("widthSlider");
        widthSlider->setGeometry(QRect(10, 190, 160, 16));
        widthSlider->setMinimum(1);
        widthSlider->setMaximum(20);
        widthSlider->setValue(2);
        widthSlider->setOrientation(Qt::Horizontal);
        serverUrlEdit = new QLineEdit(widget);
        serverUrlEdit->setObjectName("serverUrlEdit");
        serverUrlEdit->setGeometry(QRect(10, 220, 160, 24));
        playerNameEdit = new QLineEdit(widget);
        playerNameEdit->setObjectName("playerNameEdit");
        playerNameEdit->setGeometry(QRect(10, 250, 160, 24));
        btnConnect = new FancyButton(widget);
        btnConnect->setObjectName("btnConnect");
        btnConnect->setGeometry(QRect(10, 280, 75, 24));
        btnDisconnect = new FancyButton(widget);
        btnDisconnect->setObjectName("btnDisconnect");
        btnDisconnect->setEnabled(false);
        btnDisconnect->setGeometry(QRect(95, 280, 75, 24));
        roomNameEdit = new QLineEdit(widget);
        roomNameEdit->setObjectName("roomNameEdit");
        roomNameEdit->setGeometry(QRect(10, 315, 160, 24));
        btnCreateRoom = new FancyButton(widget);
        btnCreateRoom->setObjectName("btnCreateRoom");
        btnCreateRoom->setGeometry(QRect(10, 345, 160, 24));
        roomStatusLabel = new QLabel(widget);
        roomStatusLabel->setObjectName("roomStatusLabel");
        roomStatusLabel->setGeometry(QRect(10, 380, 160, 24));
        roomStatusLabel->setAlignment(Qt::AlignCenter);
        btnJoinRoom = new FancyButton(widget);
        btnJoinRoom->setObjectName("btnJoinRoom");
        btnJoinRoom->setGeometry(QRect(10, 410, 160, 24));
        btnGameStart = new FancyButton(widget);
        btnGameStart->setObjectName("btnGameStart");
        btnGameStart->setGeometry(QRect(10, 475, 160, 24));
        btnRoundStart = new FancyButton(widget);
        btnRoundStart->setObjectName("btnRoundStart");
        btnRoundStart->setGeometry(QRect(10, 505, 160, 24));
        btnGameEnd = new FancyButton(widget);
        btnGameEnd->setObjectName("btnGameEnd");
        btnGameEnd->setGeometry(QRect(10, 535, 160, 24));
        wordLabel = new QLabel(widget);
        wordLabel->setObjectName("wordLabel");
        wordLabel->setGeometry(QRect(10, 560, 160, 30));
        wordLabel->setAlignment(Qt::AlignCenter);
        btnSendChat = new FancyButton(widget);
        btnSendChat->setObjectName("btnSendChat");
        btnSendChat->setGeometry(QRect(10, 600, 160, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        colorButton->setText(QCoreApplication::translate("MainWindow", "\351\242\234\350\211\262", nullptr));
        eraserButton->setText(QCoreApplication::translate("MainWindow", "\346\251\241\347\232\256\346\223\246", nullptr));
        undoButton->setText(QCoreApplication::translate("MainWindow", "\346\222\244\345\233\236", nullptr));
        clearButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272", nullptr));
        btnLeaderboard->setText(QCoreApplication::translate("MainWindow", "\346\216\222\350\241\214\346\246\234", nullptr));
        serverUrlEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "ws://127.0.0.1:9000", nullptr));
        playerNameEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\347\216\251\345\256\266\345\220\215\347\247\260", nullptr));
        btnConnect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245", nullptr));
        btnDisconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200", nullptr));
        roomNameEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\346\210\277\351\227\264\345\220\215", nullptr));
        btnCreateRoom->setText(QCoreApplication::translate("MainWindow", "\345\210\233\345\273\272\346\210\277\351\227\264", nullptr));
        roomStatusLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\252\345\212\240\345\205\245\346\210\277\351\227\264", nullptr));
        roomStatusLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "QLabel {\n"
"    background-color: rgba(52, 152, 219, 200);\n"
"    color: white;\n"
"    padding: 4px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"    font-size: 11px;\n"
"}", nullptr));
        btnJoinRoom->setText(QCoreApplication::translate("MainWindow", "\345\212\240\345\205\245\346\210\277\351\227\264", nullptr));
        btnGameStart->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\270\270\346\210\217(\346\210\277\344\270\273)", nullptr));
        btnRoundStart->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\345\233\236\345\220\210(\346\210\277\344\270\273)", nullptr));
        btnGameEnd->setText(QCoreApplication::translate("MainWindow", "\347\273\223\346\235\237\346\270\270\346\210\217(\346\210\277\344\270\273)", nullptr));
        wordLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "QLabel {\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    color: #2196F3;\n"
"    border: 2px solid #2196F3;\n"
"    border-radius: 5px;\n"
"    padding: 5px;\n"
"    background-color: rgba(33, 150, 243, 0.1);\n"
"}", nullptr));
        wordLabel->setText(QCoreApplication::translate("MainWindow", "\347\224\273\346\211\213\350\257\215: ---", nullptr));
        btnSendChat->setText(QCoreApplication::translate("MainWindow", "\360\237\222\254 \350\201\212\345\244\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
