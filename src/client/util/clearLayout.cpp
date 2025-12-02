#include <QLayout>
#include <QLayoutItem>
#include <QObject>
#include "clearLayout.h"

void clearLayout(QLayout *layout)
{
    if (!layout)
        return;

    while (layout->count() > 0)
    {
        QLayoutItem *item = layout->takeAt(0);

        if (QWidget *widget = item->widget())
        {
            widget->hide();        // optional
            widget->deleteLater(); // safest way to delete widgets
        }

        if (QLayout *childLayout = item->layout())
        {
            clearLayout(childLayout); // recursive delete for nested layouts
        }

        delete item; // delete QLayoutItem
    }
}