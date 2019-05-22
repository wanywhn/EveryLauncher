#include "detailedtext.h"
#include <Model/recollmodel.h>

DetailedW::DetailedW(QWidget *parent) : QWidget(parent)
{
}

void DetailedW::showDoc()
{

}

void DetailedW::setHighlightData(HighlightData hl)
{
    this->m_hData=hl;
}

void DetailedW::setIndex(const QModelIndex &value)
{
    index = value;
}

bool DetailedW::supportMimeType(std::string &m)
{
    return setSupportedMimeType.find(m)!=setSupportedMimeType.cend();
}
