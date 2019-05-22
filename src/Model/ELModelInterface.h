//
// Created by tender on 19-5-22.
//

#ifndef EVERYLAUNCHER_ELMODELINTERFACE_H
#define EVERYLAUNCHER_ELMODELINTERFACE_H

#include <string>
#include <QAbstractListModel>

class ELModelInterface {
public :
    virtual void search(std::string &)=0;
};

#endif //EVERYLAUNCHER_ELMODELINTERFACE_H
