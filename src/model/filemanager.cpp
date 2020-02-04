#include "filemanager.h"

#include <QStandardPaths>

FileManager::FileManager(QList<Component*> components, QList<Connection*> connections) :
    _components(components), _connections(connections)
{
}

void FileManager::saving(void)
{
    _dirFilePath.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QFile _actualFile;
    _actualFile.setFileName(QFileDialog::getSaveFileName  (nullptr,"Speichern", _dirFilePath.absolutePath(), "Json (*.json);;Text (*.txt)"));

    if (_actualFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&_actualFile);
        out << createSaveData();
        _actualFile.close();
    }
}

QString FileManager::createSaveData(void)
{
    QJsonDocument json;
    QJsonArray array;
    for (Component* c : _components)
    {
        if (c->getComponentType() == Component::Resistor)
        {
            array.append(saveResistor(c));
        }
        else
        {
            array.append(savePowerSupply(c));
        }
    }

    for (Connection* c : _connections)
    {
        array.append(saveConnection(c));
    }

    json.setArray(array);
    return json.toJson();
}

QJsonObject FileManager::saveResistor(Component *component)
{
    QJsonObject r ;
    r.insert("type", "Resistor");
    r.insert("_name", component->getName());
    r.insert("_xPos", component->getXPosition());
    r.insert("_yPos", component->getYPosition());
    r.insert("_orientation", component->isVertical());
    r.insert("_resistance",component->getValue());
    return r;
}

QJsonObject FileManager::savePowerSupply(Component *component)
{
    QJsonObject ps;
    ps.insert("type", "PowerSupply");
    ps.insert("_name", component->getName());
    ps.insert("_xPos.", component->getXPosition());
    ps.insert("_yPos.", component->getYPosition());
    ps.insert("_orientation", component->isVertical());
    return ps;
}

QJsonObject FileManager::saveConnection(Connection *connection)
{
    //TDO: Ports hinzufügen
    QJsonObject c;
    c.insert("type", "Connection");
    c.insert("_componentA", connection->getComponentA()->getName());
    c.insert("_componentB", connection->getComponentB()->getName());
    c.insert("_portA", connection->getPortA());
    c.insert("_portB", connection->getPortB());
    return c;
}
