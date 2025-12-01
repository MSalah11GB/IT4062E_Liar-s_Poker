#include <QWidget>
#include <QLayout>
#include <QLayoutItem>
#include <QObject>
#include "clearLayout.h"

void clearLayout(QWidget *window)
{
    // Remove layout if exists
    if (window->layout())
    {
        QLayoutItem *item;
        while ((item = window->layout()->takeAt(0)) != nullptr)
        {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }
        delete window->layout();
    }

    // Also remove children NOT in layout
    for (QObject *child : window->children())
    {
        QWidget *w = qobject_cast<QWidget *>(child);
        if (w)
            w->deleteLater();
    }
}