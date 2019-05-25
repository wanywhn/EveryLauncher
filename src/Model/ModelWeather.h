//
// Created by tender on 19-5-22.
//

#ifndef EVERYLAUNCHER_MODELWEATHER_H
#define EVERYLAUNCHER_MODELWEATHER_H

#include <Model/ELModelInterface.h>

class ModelWeather: public ELModelInterface{
    Q_OBJECT
public:
    ModelWeather();

    void search(QString &string) override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void clearSource() override;


private:
    QStringList sl{"1"};
};

#endif //EVERYLAUNCHER_MODELWEATHER_H
