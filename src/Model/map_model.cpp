//
// Created by tender on 19-5-23.
//

#include "map_model.h"

void map_model::search(std::string &string) {
    maplist.clear();
    maplist.append("https://www.baidu.com");
    maplist.append("https://www.google.com");

}

int map_model::rowCount(const QModelIndex &parent) const {
    maplist.size();
}

QVariant map_model::data(const QModelIndex &index, int role) const {
    if(index.row()>=maplist.size()){
        return QVariant();
    }
    QVariant var;
    switch (role){
        case ELModelInterface::ModelRoles::Role_MIME_TYPE:{
            var=MAP_TYPE;
            break;
        }
        case ELModelInterface::Role_MAP_URL:{
            var=maplist.at(index.row());
            break;
        }
        default:break;
    }
    return var;

}
