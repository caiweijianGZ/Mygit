#ifndef PRODUCTCREATOR_H
#define PRODUCTCREATOR_H

#include <QMainWindow>
#include "config.h"
#include "product/DCD9500/DCD9500_MiniFSUTool.h"

class productCreator
{
public:
    productCreator();
    QMainWindow *createProduct();

};

#endif // PRODUCTCREATOR_H
