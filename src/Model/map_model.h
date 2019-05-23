//
// Created by tender on 19-5-23.
//

#ifndef EVERYLAUNCHER_MAP_MODEL_H
#define EVERYLAUNCHER_MAP_MODEL_H


#include "ELModelInterface.h"

class map_model: public ELModelInterface{
    Q_OBJECT
public:
    void search(std::string &string) override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    QStringList maplist;

};


#endif //EVERYLAUNCHER_MAP_MODEL_H
