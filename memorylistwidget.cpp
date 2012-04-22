#include "memorylistwidget.h"

MemoryListWidget::MemoryListWidget(QWidget *parent) :
    QTableWidget(parent)
{
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);
    this->setColumnCount(4);
    QStringList labels;
    labels << tr("Address") << tr("Offset") << tr("Hex Value") << tr("Decimal"); //<< tr("Description");
    this->setHorizontalHeaderLabels(labels);
    this->verticalHeader()->hide();
    this->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    this->setShowGrid(true);

    this->connect(this, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(on_CurrentCellChanged(int,int,int,int)));
    this->connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(on_CellDoubleClicked(int,int)));

    this->blockSignals(true);
    //this->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MemoryListWidget::scanUpdated(MemoryScanner *scan)
{
    int matchcount = scan->getMatchCount();
    if (matchcount > 1000)
    {
        QMessageBox m;
        QString msg = "There are " + QString::number(matchcount) +  " cells in the search.\n\n";
        msg += "Would you like to load the values now?\n\n";
        msg += "Warning: this action is not reccomended until search is narrowed down.";
        m.setText(msg);
        m.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        m.setDefaultButton(QMessageBox::No);
        m.setWindowTitle("Large Data Size");

        if (m.exec() != QMessageBox::Yes)
            return;
    }

    static char buffer[256];
    unsigned int offset;
    MemoryCell *mb = scan->getHead();

    this->clear();
    this->setRowCount(0);

    int row = 0;
    while (mb)
    {
        for (offset = 0; offset < mb->getSize(); offset += mb->getSearchDataSize())
        {
            if (mb->isInSearch(offset))
            {
                this->insertRow(row);

                //unsigned int val = peek (mb->hProc, mb->data_size, (unsigned int)mb->addr + offset);
                unsigned int val = mb->peek(offset);

                sprintf(buffer, "0x%08x", ((unsigned int)mb->getBaseAddress()) + offset);
                QTableWidgetItem *address = new QTableWidgetItem(QString(buffer));
                address->setFlags(Qt::NoItemFlags);

                sprintf(buffer, "0x%08x", val);
                QTableWidgetItem *hexVal = new QTableWidgetItem(QString(buffer));

                QTableWidgetItem *decVal = new QTableWidgetItem(QString::number(val));
                this->setItem(row, 0, address);
                this->setItem(row, 2, hexVal);
                this->setItem(row, 3, decVal);


                //qDebug ("0x%08x: 0x%08x (%d) \r\n", ((unsigned int)mb->getBaseAddress()) + offset, val, val);
            }
        }

        mb = mb->getNext();
    }
}
