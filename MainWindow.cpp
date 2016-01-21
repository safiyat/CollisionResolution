#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
//	on_sdButton_clicked();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::cellClicked(int y, int x)
{
	QTableWidgetItem *item = new QTableWidgetItem(tr("X"));
	item->setTextAlignment(Qt::AlignVCenter);
	item->setTextAlignment(Qt::AlignHCenter);
	if(ui->tableWidget->item(y, x) == 0)
		ui->tableWidget->setItem(y, x, item);
	else
	{
		ui->tableWidget->takeItem(y, x);
	/*
		if(ui->tableWidget->item(y, x))
			ui->statusBar->showMessage("Item is not NULL");
		else
	*/
		ui->statusBar->showMessage("Removed item.");
	}

}

void MainWindow::genTable()
{
	int r, c;
	r = ui->pipeStageSpinBox->text().toInt();
	c = ui->clockCycleSpinBox->text().toInt();
	ui->tableWidget->setRowCount(r);
	ui->tableWidget->setColumnCount(c);
	ui->tableWidget->setEnabled(TRUE);

	QStringList list;
	for(int i = 0; i < r; i++)
	{
		list.append(tr("S%1").arg(r - i));
	}
	ui->tableWidget->setVerticalHeaderLabels(list);

	list.clear();

	for(int i = 1; i <= c; i++)
	{
		list.append(tr("T%1").arg(i));
	}
	ui->tableWidget->setHorizontalHeaderLabels(list);
}

void MainWindow::calculate()
{
	QStringList list;
	QString fl(ui->tableWidget->columnCount(), '0');
//	int table[ui->tableWidget->rowCount()][ui->tableWidget->columnCount()];
/****Forbidden Latencies****/
	for(int i = ui->tableWidget->rowCount() - 1; i >= 0; i--)
	{
		list.clear();
		for(int j = 0; j < ui->tableWidget->columnCount(); j++)
		{
			if(ui->tableWidget->item(i, j))
			{
//				table[i][j] = 1;
				list.append(tr("%1").arg(j));
			}
//			else
//				table[i][j] = 0;
		}
		for(int j = list.size() - 1; j >= 0; j--)
		{
			int c1 = list.at(j).toInt();
			for(int k = list.size() - 1; k >= 0; k--)
			{
				int c2 = list.at(k).toInt();
				int diff = c1 - c2;
				if(diff < 0)
					continue;
				if((fl.at(diff) == '0'))
					fl[diff] = '1';
			}
		}
	}
	QString forbid_latency("");
	for(int i = 0; i < ui->tableWidget->columnCount(); i++)
	{
		if(fl.at(i) == '1')
		{
			if(forbid_latency.isEmpty())
				forbid_latency.append(tr("%1").arg(i));
			else
				forbid_latency.append(tr(", %1").arg(i));
		}
	}
	ui->fbLabel->setText(forbid_latency);
/****Collision Vector****/
	ui->cvLabel->setText(fl);
/****States****/
	QStringList states;
	states.append(fl);
	for(int k = 0; k < states.size(); k++)
	{
		for(int i = 0; i < fl.length(); i++)
		{
			if(fl.at(i) == '0')
			{
				QString shiftedState, newState;
				shiftedState = fl.right(fl.length() - i);
				shiftedState += QString(i, '0');
				for(int j = 0; j < shiftedState.length(); j++)
				{
					if((shiftedState.at(j) == '1') || (fl.at(j) == '1'))
						newState += "1";
					else
						newState += "0";
				}
//				QMessageBox::information(this, "New State", tr("New State %1").arg(newState));
				if(!states.contains(newState))
					states.append(newState);
			}
		}
	}
	ui->stateList->addItems(states);
/****Greedy Cycles****/
//	QStringList g_cycles;
	QString minimalCycle;
	int pos, cost, count;
	float mal = -1;
	for(int k = 0; k < states.size(); k++)
	{
		QString initial(states.at(k));
		QString greedyS(initial);
		QString al("(");;
		count = cost = 0;
		do
		{
			greedyS = greedyShift(greedyS, &pos);
			count++;
			cost += pos;
			al.append(tr(" %1").arg(pos));

		}while(greedyS != "NULL" && greedyS != initial);
		/*
		if(greedyS == "NULL")
		{
			count++;
			cost += pos;
		}
		*/
		al.append(" )");
		if(mal == -1)
		{
			minimalCycle = al;
			mal = (float)cost / count;
		}
		else if(mal > ((float)cost / count))
		{
			minimalCycle = al;
			mal = (float)cost / count;
		}
	}
	ui->gcLabel->setText(minimalCycle);
/****MAL****/
	ui->malLabel->setText(tr("%1").arg(mal));

/****Throuput****/
	ui->tLabel->setText(tr("%1").arg(1 / mal));
	/*
	for(int i = 0; i < minimalCycle.length(); i++)
	{
		if(minimalCycle.at(i).isDigit())
		{

		}
	}
	*/
}

QString MainWindow::newStateShift(QString parent, int shift)
{
			QString shiftedState, newState;
			shiftedState = parent.right(parent.length() - shift);
			shiftedState += QString(shift, '0');
			for(int j = 0; j < shiftedState.length(); j++)
			{
				if((shiftedState.at(j) == '1') || (parent.at(j) == '1'))
					newState += "1";
				else
					newState += "0";
			}
			return newState;
}

QString MainWindow::greedyShift(QString string, int *index)
{
	QString result("");
	for(int i = 0; i < string.length(); i++)
	{
		if(string.at(i) == '0')
		{
			result = newStateShift(string, i);
			*index = i;
			break;
		}
	}
	if(result.isEmpty())
	{
		result = "NULL";
		*index = string.length();
	}
	return result;
}

void MainWindow::on_sdButton_clicked()
{
	QDialog *sd = new QDialog;
	sd->setWindowTitle("State Diagram");
	QGraphicsScene *scene = new QGraphicsScene;

	QLabel l("100101");
	l.resize(l.sizeHint());

	scene->addEllipse(2, 2, l.width() + 10, l.height() + 2);
	scene->addText(l.text());

	l.setText("110011");
	scene->addEllipse(2, 30, l.width() + 10, l.height() + 2);
	scene->addText(l.text());

	QGraphicsView *diagram = new QGraphicsView(scene);
//	diagram->setScene(scene);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(diagram);
	sd->setLayout(layout);
	sd->show();
}
