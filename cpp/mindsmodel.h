#ifndef __MINDSMODEL_H__
#define __MINDSMODEL_H__

#include <QAbstractItemModel>
#include <QRgb>
#include "minds.h"

class MindsModel : public QAbstractListModel
{
Q_OBJECT
public:
    MindsModel(QObject *parent=NULL);
    virtual Qt::ItemFlags flags(const QModelIndex&) const;
    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex&, int) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool addPossiblePlayer(const QString &player_name,const QString &module_name,QRgb player_color);
    void createPlayers(World *world) const;
public slots:
    void updateData();
protected:
    typedef std::set<int> Enabled;
    typedef std::vector<QRgb> Colors;
    Enabled enabled;
    Colors colors;
    const PythonMinds::Names &names;
};

#endif

