#include "createworldwidget.h"

#include <QtGui>
#include "minds.h"

CreateWorldWidget::CreateWorldWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    {
        QLabel *label = new QLabel("world size",this);
        layout->addWidget(label);

        world_width = new QSpinBox(this);
        world_width->setRange(50,500);
        world_width->setValue(300);
        layout->addWidget(world_width);

        world_height = new QSpinBox(this);
        world_height->setRange(50,500);
        world_height->setValue(300);
        layout->addWidget(world_height);
    }

    {
        QLabel *label = new QLabel("number of plant",this);
        layout->addWidget(label);
        
        world_plants = new QSpinBox(this);
        world_plants->setRange(5,50);
        world_plants->setValue(12);
        layout->addWidget(world_plants);
    }

    {
        QPushButton *button = new QPushButton("create world",this);
        connect(button,SIGNAL(clicked()),SLOT(createWorld()));
        layout->addWidget(button);
    }

    layout->addStretch();
    setMinimumWidth(300);
}

void CreateWorldWidget::createWorld()
{
    World *world = new World(world_width->value(),world_height->value(),world_plants->value());
    emit worldCreated(world);
}

