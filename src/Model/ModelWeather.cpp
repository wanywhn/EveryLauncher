//
// Created by tender on 19-5-22.
//

#include "ModelWeather.h"

void ModelWeather::search(std::string &string) {

}

int ModelWeather::rowCount(const QModelIndex &parent) const {
//    return 0;
    return sl.size();
}

QVariant ModelWeather::data(const QModelIndex &index, int role) const {
    if(!index.isValid()||index.row()>=sl.size()){
        return {};
    }
    if(role==ELModelInterface::Role_MIME_TYPE){
        return WEATHER_TYPE;

    }
    return sl.at(index.row());
}

ModelWeather::ModelWeather() {
    this->setObjectName("WeatherModel");
    this->setDisplayName(tr("天气"));
}

