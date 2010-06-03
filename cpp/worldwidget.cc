#include "worldwidget.h"

float normalize(float value, float value_max) {
    if (value<0) return 0;
    if (value>value_max) return 1.;
    return value/value_max;
}

WorldWidget::WorldWidget(const World &world, QWidget *parent) : QWidget(parent), world(world), image_need_update(true),
                                                                image_map(QSize(world.width,world.height),QImage::Format_RGB32),
                                                                image_energy(QSize(world.width,world.height),QImage::Format_ARGB32),
                                                                image_overlay(QSize(world.width,world.height),QImage::Format_ARGB32)
{
    setMinimumSize(300,300);
}

void WorldWidget::imageNeedUpdate()
{
    image_need_update = true;
    repaint();
}

void WorldWidget::paintEvent(QPaintEvent *event)
{
    if (image_need_update) {
        for (int x=0; x<world.width; x++) for (int y=0; y<world.height; y++) {
            float altitude = world.altitude.get(x,y);
            float normalized = normalize(altitude,altitude_max);
            uint color = qRgb(255*normalized,127*normalized,63*normalized);
            image_map.setPixel(x,y,color);
        }

        for (int x=0; x<world.width; x++) for (int y=0; y<world.height; y++) {
            float energy = world.energy.get(x,y);
            float normalized = normalize(energy,energy_max);
            normalized = world.occupied.get(x,y); //FIXME debug occupied
            uint color = qRgba(0,127,255,255*normalized);
            image_energy.setPixel(x,y,color);
        }

        image_overlay.fill(qRgba(0,0,0,0));
        for (World::Plants::const_iterator i=world.plants.begin(); i!=world.plants.end(); i++) {
            const World::Plant *plant = *i;
            image_overlay.setPixel(plant->position.x,plant->position.y,qRgb(0,255,0));
        }

        for (World::Players::const_iterator i=world.players.begin(); i!=world.players.end(); i++) {
            const World::Player *player = *i;
            for (World::Player::Agents::const_iterator j=player->agents.begin(); j!=player->agents.end(); j++) {
                const World::Player::Agent *agent = *j;
                image_overlay.setPixel(agent->position.x,agent->position.y,player->color);
            }
        }
        image_need_update = false;
    }

    Q_UNUSED(event);
    QPainter painter(this);
    painter.translate(rect().center());

    float ratio_width  = static_cast<float>(rect().width())/world.width;
    float ratio_height = static_cast<float>(rect().height())/world.height;
    float ratio = ratio_width < ratio_height ? ratio_width : ratio_height;
    painter.scale(ratio,ratio);
    painter.translate(-world.width/2.,-world.height/2.);
    painter.drawImage(QPoint(),image_map);
    painter.drawImage(QPoint(),image_energy);
    painter.drawImage(QPoint(),image_overlay);

}
