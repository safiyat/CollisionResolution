#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QGraphicsView>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
	public slots:
		void cellClicked(int, int);
		void genTable();
		void calculate();
		void on_sdButton_clicked();

	private:
		Ui::MainWindow *ui;
		QString newStateShift(QString parent, int shift);
		QString greedyShift(QString string, int *index);
};

#endif // MAINWINDOW_H
