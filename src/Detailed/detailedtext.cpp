#include "detailedtext.h"
#include <recollmodel.h>

DetailedW::DetailedW(QWidget *parent) : QWidget(parent)
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
    return setSupportedMimeType.contains(m);
}
