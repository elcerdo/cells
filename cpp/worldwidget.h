#ifndef __WORLDWIDGET_H__
#define __WORLDWIDGET_H__

#include <QWidget>
#include <QPainter>
#include "world.h"

class WorldWidget : public QWidget
{
Q_OBJECT
public:
    WorldWidget(QWidget *parent = NULL);
    virtual void paintEvent(QPaintEvent *event);
public slots:
    void setWorld(const World *world);
    void imageNeedUpdate();
protected:
    static const float altitude_max = 10.;
    static const float energy_max = 100.;
    const World *world;
    bool image_need_update;
    QImage image_map;
    QImage image_energy;
    QImage image_overlay;
};

#endif
