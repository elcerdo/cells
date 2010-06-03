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
    virtual void paintEvent(QPaintEvent *event);
public slots:
    void imageNeedUpdate();
protected:
    static const float altitude_max = 5.;
    static const float energy_max = 10.;
    const World &world;
    bool image_need_update;
    QImage image_map;
    QImage image_energy;
    QImage image_overlay;
};

#endif
