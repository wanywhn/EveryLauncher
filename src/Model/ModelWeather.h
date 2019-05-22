//
// Created by tender on 19-5-22.
//

#ifndef EVERYLAUNCHER_MODELWEATHER_H
#define EVERYLAUNCHER_MODELWEATHER_H

#include <Model/ELModelInterface.h>

class ModelWeather: public ELModelInterface,public QAbstractListModel{
public:
    void search(std::string &string) override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

};

#endif //EVERYLAUNCHER_MODELWEATHER_H
