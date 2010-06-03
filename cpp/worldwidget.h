#ifndef __WORLDWIDGET_H__
#define __WORLDWIDGET_H__

#include <QWidget>
#include <QPainter>
#include "world.h"

class WorldWidget : public QWidget
{
Q_OBJECT
public:
    WorldWidget(const World &world, QWidget *parent = NULL);

    void setImageNeedUpdate();

    virtual void paintEvent(QPaintEvent *event);

protected:
    const World &world;
    bool image_need_update;
    QImage image;
};

#endif
