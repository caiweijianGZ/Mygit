#include "MiniFSUTool.h"
#include <QApplication>

#include "productCreator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    productCreator *creator = new productCreator();
    MiniFSUTool *window = (MiniFSUTool *)creator->createProduct();
    window->extraFuncInit();
    window->show();

    return a.exec();
}
