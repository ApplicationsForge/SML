#ifndef SMLTABLEWIDGET_H
#define SMLTABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QKeyEvent>
#include <QAbstractItemView>

#include <QDebug>

class SMLTableWidget : public QTableWidget
{
    Q_OBJECT
signals:
    void addSignal();
    void editSignal(QModelIndex index);
    void eraseSignal(QModelIndexList indexes);

    void copySignal();
    void cutSignal();
    void pasteSignal();
public:
    SMLTableWidget(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *keyEvent);
private:
    QModelIndexList getRowsIndexes(QModelIndexList itemsIndexes);
};

#endif // SMLTABLEWIDGET_H
