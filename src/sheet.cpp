#include "sheet.h"

Sheet::Sheet() :
    pixmap(nullptr),
    scene(nullptr),
    scaled(false)
{}

Sheet::~Sheet()
{
    clear();
}

void Sheet::clear()
{
    delete scene;
    delete pixmap;
}
