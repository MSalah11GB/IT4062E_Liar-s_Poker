#include <QWidget>
#include <QLayout>
#include <QLayoutItem>
#include <QObject>
#include "deleteWidgetLayout.h"

void deleteWidgetLayout(QWidget *widget)
{
    // Remove layout if exists
    if (widget->layout())
    {
        QLayoutItem *item;
        while ((item = widget->layout()->takeAt(0)) != nullptr)
        {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }
        delete widget->layout();
    }

    // Also remove children NOT in layout
    for (QObject *child : widget->children())
    {
        QWidget *w = qobject_cast<QWidget *>(child);
        if (w)
            w->deleteLater();
    }
}