#ifndef SHEET_H
#define SHEET_H

#include <QPixmap>
#include <QGraphicsScene>

class Sheet
{
public:
    Sheet() :
        pixmap(nullptr),
        scene(nullptr),
        scaled(false)
    {}
    ~Sheet() { this->clear(); }

    void clear()
    {
        delete scene;
        delete pixmap;
    }

    QPixmap* pixmap;
    QGraphicsScene* scene;

    QString name;
    bool scaled;
};

#endif // SHEET_H
