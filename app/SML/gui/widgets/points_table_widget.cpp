#include "points_table_widget.h"

PointsTableWidget::PointsTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PointsTableWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    QModelIndexList selectedItemsIndexes = this->selectedIndexes();
    if(selectedItemsIndexes.size() > 0)
    {
        QModelIndexList selectedRowsIndexes = getRowsIndexes(selectedItemsIndexes);
        qSort(selectedRowsIndexes);

        int keyPressed = keyEvent->key();

        switch (keyPressed) {
        case Qt::Key_Return:
        {
            keyReturnPressed(selectedRowsIndexes);
            break;
        }
        case Qt::Key_Backspace:
        {
            keyBackspacePressed(selectedRowsIndexes);
            break;
        }
        case Qt::Key_Up:
        {
            keyUpPressed(selectedRowsIndexes);
            break;
        }
        case Qt::Key_Down:
        {
            keyDownPressed(selectedRowsIndexes);
            break;
        }
        case Qt::Key_A:
        {
            if(keyEvent->modifiers() == Qt::ControlModifier)
            {
                keysCtrlAPressed();
            }
            break;
        }
        default:
        {
            break;
        }
        }
    }
    else
    {
        selectFirstRow();
    }
}

QModelIndexList PointsTableWidget::getRowsIndexes(QModelIndexList itemsIndexes)
{
    QModelIndexList rowsIndexes = {*itemsIndexes.begin()};
    int currentItemRow = rowsIndexes.begin()->row();
    for(int i = 1; i < itemsIndexes.size(); i++)
    {
        if(itemsIndexes[i].row() != currentItemRow)
        {
            rowsIndexes.push_back(itemsIndexes[i]);
            currentItemRow = itemsIndexes[i].row();
        }
    }
    return rowsIndexes;
}

void PointsTableWidget::keyReturnPressed(QModelIndexList selectedRowsIndexes)
{
    int rowsCount = this->rowCount();
    if(selectedRowsIndexes.first().row() >= 0 && selectedRowsIndexes.first().row() < rowsCount)
    {
        emit editSignal(*selectedRowsIndexes.begin());
    }
}

void PointsTableWidget::keyBackspacePressed(QModelIndexList selectedRowsIndexes)
{
    emit eraseSignal(selectedRowsIndexes);
}

void PointsTableWidget::keyUpPressed(QModelIndexList selectedRowsIndexes)
{
    int rowsCount = this->rowCount();
    int firstSelectedRow = selectedRowsIndexes.first().row();
    if(firstSelectedRow > 0)
    {
        selectRow(firstSelectedRow - 1);
    }
    else
    {
        if(firstSelectedRow == 0)
        {
            selectRow(rowsCount - 1);
        }
    }
}

void PointsTableWidget::keyDownPressed(QModelIndexList selectedRowsIndexes)
{
    int rowsCount = this->rowCount();
    int lastSelectedRow = selectedRowsIndexes.back().row();
    if(lastSelectedRow >= 0 && lastSelectedRow < (rowsCount - 1))
    {
        selectRow(lastSelectedRow + 1);
    }
    else
    {
        if(lastSelectedRow == (rowsCount - 1))
        {
            selectFirstRow();
        }
    }
}

void PointsTableWidget::keysCtrlAPressed()
{
    selectAll();
}

void PointsTableWidget::selectFirstRow()
{
    int rowsCount = this->rowCount();
    if(rowsCount > 0)
    {
        selectRow(0);
    }
}
