#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("Comparator");
	a.setApplicationVersion("0.1");
	a.setOrganizationName("Unlimited Web Works");
	a.setWindowIcon(QIcon(":/main.ico"));

	MainWindow w;
	w.show();

	return a.exec();
}
