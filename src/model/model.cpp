#include "model.h"

Model::Model(QObject *parent) : QObject(parent)
{

}
void Model::paintObject()
{
    emit newResistorElement();
}
