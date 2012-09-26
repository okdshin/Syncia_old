#ifdef APP_UNIT_TEST
#include "App.h"
#include <iostream>
#include <QApplication>

using namespace sy;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	auto form = new App();
	form->show();
    return app.exec();
}

#endif
