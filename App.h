#pragma once
//App:20120918
#include <iostream>
#include <QtGui>
#include <QWidget>
 #include "ui_syncia.h"

namespace sy
{

class App : public QMainWindow {
	Q_OBJECT
public:
    App(QWidget* parent = 0){
		ui.setupUi(this);	
	}
    ~App(){}

private:
	Ui::MainWindow ui;
};
}

