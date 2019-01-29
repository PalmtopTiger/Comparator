#ifndef SHEET_H
#define SHEET_H

#include <QPixmap>
#include <QGraphicsScene>

class Sheet
{
public:
    Sheet();
    ~Sheet();

    void clear();

    QPixmap* pixmap;
    QGraphicsScene* scene;

    QString name;
    bool scaled;
};

#endif // SHEET_H
