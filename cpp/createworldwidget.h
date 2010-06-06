#ifndef __CREATEWORLDWIDGET_H__
#define __CREATEWORLDWIDGET_H__

#include <QSpinBox>
#include "world.h"

class CreateWorldWidget : public QWidget
{
Q_OBJECT
public:
    CreateWorldWidget(QWidget *parent=NULL);
public slots:
    void createWorld();
signals:
    void worldCreated(World *world);
protected:
    QSpinBox *world_width;
    QSpinBox *world_height;
    QSpinBox *world_plants;
};

#endif
