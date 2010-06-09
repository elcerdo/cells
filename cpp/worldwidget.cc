#include "worldwidget.h"

float normalize(float value, float value_max) {
    if (value<0) return 0;
    if (value>value_max) return 1.;
    return value/value_max;
}

WorldWidget::WorldWidget(QWidget *parent) : QWidget(parent), world(NULL), image_need_update(true)
{
    setMinimumSize(500,500);
}

void WorldWidget::imageNeedUpdate()
{
    image_need_update = true;
    repaint();
}

void WorldWidget::setWorld(const World *world_new)
{
    world = world_new;

    if (world) {
        image_map      = QImage(QSize(world->width,world->height),QImage::Format_RGB32);
        image_energy   = QImage(QSize(world->width,world->height),QImage::Format_ARGB32);
        image_occupied = QImage(QSize(world->width,world->height),QImage::Format_ARGB32);
        image_overlay  = QImage(QSize(world->width,world->height),QImage::Format_ARGB32);
    }

    imageNeedUpdate();
}

void WorldWidget::paintEvent(QPaintEvent *event)
{
    if (not world) return;

    if (image_need_update) {
        for (int x=0; x<world->width; x++) for (int y=0; y<world->height; y++) {
            float altitude = world->altitude_map.get(x,y);
            float normalized = normalize(altitude,altitude_max);
            uint color = qRgb(255*normalized,127*normalized,63*normalized);
            image_map.setPixel(x,y,color);
        }

        for (int x=0; x<world->width; x++) for (int y=0; y<world->height; y++) {
            float normalized = (world->agents_map.get(x,y)!=NULL);
            uint color = qRgba(255,0,255,255*normalized);
            image_occupied.setPixel(x,y,color);
        }

        for (int x=0; x<world->width; x++) for (int y=0; y<world->height; y++) {
            float energy = world->energy_map.get(x,y);
            float normalized = normalize(energy,energy_max);
            uint color = qRgba(0,127,255,255*normalized);
            image_energy.setPixel(x,y,color);
        }

        image_overlay.fill(qRgba(0,0,0,0));
        for (PlantInternals::const_iterator i=world->plants.begin(); i!=world->plants.end(); i++) {
            const PlantInternal *plant = *i;
            image_overlay.setPixel(plant->position.x,plant->position.y,qRgb(0,255,0));
        }

        for (Players::const_iterator i=world->players.begin(); i!=world->players.end(); i++) {
            const Player *player = *i;
            for (Player::AgentInternals::const_iterator j=player->agents.begin(); j!=player->agents.end(); j++) {
                const Player::AgentInternal *agent = *j;
                image_overlay.setPixel(agent->position.x,agent->position.y,player->color);
            }
        }
        image_need_update = false;
    }

    Q_UNUSED(event);
    QPainter painter(this);
    painter.translate(rect().center());

    float ratio_width  = static_cast<float>(rect().width())/world->width;
    float ratio_height = static_cast<float>(rect().height())/world->height;
    float ratio = ratio_width < ratio_height ? ratio_width : ratio_height;
    painter.scale(ratio,ratio);
    painter.translate(-world->width/2.,-world->height/2.);
    painter.drawImage(QPoint(),image_map);
    painter.drawImage(QPoint(),image_energy);
    painter.drawImage(QPoint(),image_occupied);
    painter.drawImage(QPoint(),image_overlay);

}
