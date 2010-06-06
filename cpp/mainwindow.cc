#include "mainwindow.h"

#include <QtGui>
#include <sstream>
#include "createworldwidget.h"

static const int message_delay = 4000;

void MainWindow::deadPlayerCallback(const World::Player &player, void *data)
{
    MainWindow *main_window = static_cast<MainWindow*>(data);
    main_window->statusBar()->showMessage(QString("%1 died RIP").arg(player.name.c_str()),message_delay);
    main_window->updateReport();
}

MainWindow::~MainWindow()
{
    if (world) {
        world_widget->setWorld(NULL);
        delete world;
        world = NULL;
    }
}

void MainWindow::setWorld(World *world_new) {
    if (world) {
        world_widget->setWorld(NULL);
        delete world;
        world = NULL;
    }

    assert(not world);
    world = world_new;
    world->callbackData = static_cast<void*>(this);
    world->deadPlayer = MainWindow::deadPlayerCallback;
    world_widget->setWorld(world);

    nworld_tick = 0;
    world_tick_time.restart();

    updateReport();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), world(NULL), nworld_tick(0), speed(1)
{
    QMenu *view_menu = menuBar()->addMenu(tr("&View"));

    QWidget *central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    QVBoxLayout *central_layout = new QVBoxLayout(central_widget);
    central_widget->setLayout(central_layout);

    {
        world_widget = new WorldWidget(central_widget);
        connect(this,SIGNAL(worldTicked()),world_widget,SLOT(imageNeedUpdate()));
        central_layout->addWidget(world_widget);
    }

    {
        world_tick_timer = new QTimer(this);
        world_tick_timer->setInterval(30);
        world_tick_timer->setSingleShot(false);
        connect(world_tick_timer,SIGNAL(timeout()),SLOT(tickWorld()));

        QPushButton *world_tick = new QPushButton("tick world",central_widget);
        world_tick->setCheckable(true);
        connect(world_tick,SIGNAL(toggled(bool)),SLOT(startTickWorldTimer(bool)));
        central_layout->addWidget(world_tick);

        QSlider *world_speed = new QSlider(Qt::Horizontal,this);
        world_speed->setRange(1,10);
        connect(world_speed,SIGNAL(valueChanged(int)),SLOT(setSpeed(int)));
        central_layout->addWidget(world_speed);
    }

    {
        QDockWidget *dock = new QDockWidget("create",this);
        addDockWidget(Qt::RightDockWidgetArea,dock);
        CreateWorldWidget *create_widget = new CreateWorldWidget(dock);
        dock->setWidget(create_widget);
        view_menu->addAction(dock->toggleViewAction());
        connect(create_widget,SIGNAL(worldCreated(World*)),SLOT(setWorld(World*)));
    }

    {
        QDockWidget *dock = new QDockWidget("report",this);
        addDockWidget(Qt::RightDockWidgetArea,dock);
        QWidget *report_widget = new QWidget(dock);
        dock->setWidget(report_widget);
        QVBoxLayout *report_layout = new QVBoxLayout(report_widget);
        report_widget->setLayout(report_layout);
        view_menu->addAction(dock->toggleViewAction());

        QPushButton *report_update = new QPushButton("update report",report_widget);
        connect(report_update,SIGNAL(clicked()),SLOT(updateReport()));
        report_layout->addWidget(report_update);

        report_label = new QLabel(report_widget);
        report_layout->addWidget(report_label);
        updateReport();

        report_layout->addStretch();
        report_widget->setMinimumWidth(300);
    }

    statusBar()->showMessage("welcome to cells!",message_delay);
}

void MainWindow::setSpeed(int new_speed)
{
    if (new_speed<1 or new_speed>10) return;
    speed = new_speed;
    statusBar()->showMessage(QString("speed x%1").arg(speed),message_delay);
    world_tick_timer->setInterval(30./speed);
}

void MainWindow::startTickWorldTimer(bool start)
{
    if (start) {
        nworld_tick = 0;
        world_tick_time.restart();
        world_tick_timer->start();
    } else {
        world_tick_timer->stop();
        float elapsed = world_tick_time.elapsed()/1000.;
        statusBar()->showMessage(QString("simulated %1 ticks in %2s %3ticks/s").arg(nworld_tick).arg(elapsed,0,'f',1).arg(nworld_tick/elapsed,0,'f',1),message_delay);
    }
}

void MainWindow::tickWorld()
{
    if (not world) return;
    nworld_tick++;
    world->tick();
    if (nworld_tick%speed == 0) emit worldTicked();
    if (nworld_tick%(10*speed) == 0) updateReport();
}

void MainWindow::updateReport()
{
    if (not world) {
        report_label->setText("no world");
        return;
    }
    std::stringstream ss;
    world->print(ss);
    report_label->setText(QString::fromStdString(ss.str()));
}
