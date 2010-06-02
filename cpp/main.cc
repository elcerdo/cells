#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <cassert>

float rand_uniform() { return static_cast<float>(rand())/RAND_MAX; }

template <typename T>
struct Map
{
    Map(int width, int height) : width(width), height(height), size(width*height), flat(NULL) {
        flat = new T[size];
    }
    ~Map() {
        delete [] flat;
    }

    T &get(int x, int y) {
        assert(x>=0 and x<width);
        assert(y>=0 and y<height);
        return flat[x*height+y];
    }

    const T &get(int x, int y) const {
        assert(x>=0 and x<width);
        assert(y>=0 and y<height);
        return flat[x*height+y];
    }

    const int width,height,size;
    T *flat;
};

struct World
{
    World(int width=300, int height=300) : width(width), height(height), altitude(width,height) {
            Map<float> altitude_tmp(width+2,height+2);
            for (int i=0; i<altitude_tmp.size; i++) { altitude_tmp.flat[i] = rand_uniform(); }

            for (int x=0; x<width; x++) for (int y=0; y<height; y++) {
                float mean = altitude_tmp.get(x+1,y+1) + altitude_tmp.get(x,y+1) + altitude_tmp.get(x+2,y+1) + altitude_tmp.get(x+1,y) + altitude_tmp.get(x+1,y+2);
                mean /= 5;
                altitude.get(x,y) = mean;
            }
    }

    const int width,height;
    Map<float> altitude;
};

class WorldWidget : public QWidget
{
public:
    WorldWidget(const World &world, QWidget *parent = NULL) : QWidget(parent), world(world), image_need_update(true), image(QSize(world.width,world.height),QImage::Format_RGB32) {
        setMinimumSize(300,300);
        resize(700,700);
    }

    void setImageNeedUpdate() {
        image_need_update = true;
        repaint();
    }

    virtual void paintEvent(QPaintEvent *event) {
        if (image_need_update) {
            for (int x=0; x<world.width; x++) for (int y=0; y<world.height; y++) {
                float altitude = world.altitude.get(x,y);
                uint pixel = qRgb(255*altitude,127*altitude,63*altitude);
                image.setPixel(x,y,pixel);
            }
            image_need_update = false;
        }

        Q_UNUSED(event);
        QPainter painter(this);
        painter.translate(rect().center());
        painter.scale(2.,2.);
        QPoint image_pos = QPoint(-image.width()/2.,-image.height()/2.);
        painter.drawImage(image_pos,image);
    }

protected:
    const World &world;
    bool image_need_update;
    QImage image;
};



int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication app(argc,argv);
    World world;
    WorldWidget widget(world);

    widget.show();

    return app.exec();
}

