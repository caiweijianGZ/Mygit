#include "productCreator.h"

productCreator::productCreator()
{

}

QMainWindow *productCreator::createProduct()
{
#ifdef DCD_9500
    return new DCD9500_MiniFSUTool();
#endif

    return NULL;
}
