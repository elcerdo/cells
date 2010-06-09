#include "mindsmodel.h"

#include <QColor>

MindsModel::MindsModel(QObject *parent) : QAbstractListModel(parent) {}

int MindsModel::rowCount(const QModelIndex &) const { 
    return Python::getMindCount();
}

void MindsModel::updateData()
{
    QModelIndex tl = createIndex(0,0);
    QModelIndex br = createIndex(Python::getMindCount()-1,0);
    emit dataChanged(tl,br);
}

Qt::ItemFlags MindsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    return flags;
}

QVariant MindsModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::DisplayRole) {
        const Python::MindName &iter = Python::getMindName(index.row());
        QString prout = QString("%1 as %2").arg(iter.first.c_str()).arg(iter.second.c_str());
        return QVariant(prout);
    }

    if (role==Qt::CheckStateRole) {
        if (enabled.find(index.row())!=enabled.end()) return QVariant(Qt::Checked);
        else return QVariant(Qt::Unchecked);
    }

    if (role==Qt::BackgroundRole) {
        return QVariant(QColor(colors[index.row()]));
    }

    if (not (role==Qt::DecorationRole or role==Qt::FontRole or role==Qt::ForegroundRole or role==Qt::TextAlignmentRole or Qt::SizeHintRole)) { qDebug("unknom role %d",role); }

    return QVariant();
}

bool MindsModel::addPossiblePlayer(const QString &player_name,const QString &module_name,QRgb player_color)
{
    if (Python::loadMind(player_name.toStdString(),module_name.toStdString())) {
        colors.push_back(player_color);
        updateData();
        return true;
    }
    return false;
}

void MindsModel::createPlayers(World *world) const
{
    //qDebug("creating players");

    for (Enabled::const_iterator i=enabled.begin(); i!=enabled.end(); i++) {
        const Python::MindName &names = Python::getMindName(*i);
        //qDebug("\t%s as %s",player_name.c_str(),module_name.c_str());
        world->addPlayer(names.first,colors[*i],Python::mind);
    }
}

bool MindsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role==Qt::CheckStateRole) {
        if (value.toBool()) enabled.insert(index.row());
        else enabled.erase(index.row());
        updateData();
        return true;
    }
    qDebug("setting %d",role);
    return false;
}

