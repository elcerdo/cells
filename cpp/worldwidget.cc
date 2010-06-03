#include "worldwidget.h"

WorldWidget::WorldWidget(const World &world, QWidget *parent) : QWidget(parent), world(world), image_need_update(true), image(QSize(world.width,world.height),QImage::Format_RGB32)
{
    setMinimumSize(300,300);
}

void WorldWidget::setImageNeedUpdate()
{
    image_need_update = true;
    repaint();
}

void WorldWidget::paintEvent(QPaintEvent *event)
{
    if (image_need_update) {
        for (int x=0; x<world.width; x++) for (int y=0; y<world.height; y++) {
            float altitude = world.altitude.get(x,y);
            uint color = qRgb(255*altitude,127*altitude,63*altitude);
            image.setPixel(x,y,color);
        }
        for (World::Plants::const_iterator i=world.plants.begin(); i!=world.plants.end(); i++) {
            const World::Plant *plant = *i;
            image.setPixel(plant->position.x,plant->position.y,qRgb(0,255,0));
        }
        for (World::Players::const_iterator i=world.players.begin(); i!=world.players.end(); i++) {
            const World::Player *player = *i;
            for (World::Player::Agents::const_iterator j=player->agents.begin(); j!=player->agents.end(); j++) {
                const World::Player::Agent *agent = *j;
                image.setPixel(agent->position.x,agent->position.y,player->color);
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
    painter.translate(-image.width()/2.,-image.height()/2.);
    painter.drawImage(QPoint(),image);

}
