#include "filemanager.h"

FileManager::FileManager(QList<Component*>& components, QList<Connection*>& connections) :
        _components(&components), _connections(&connections)
{
}

/*!
* \brief Speichert das Netzwerk ab.
*
* Die Methode speichert die vorhandene Schaltung ab.
* Zu Beginn wird geprüft ob das Netzwerk bereits abgespeichert wurde.
* Ist noch keine Datei vorher erstellt worden, wird diese zuerst neu erstellt.
* Dazu wird mit Hilfe von QStandardPaths der Fad angegeben, wo die Datei abgelegt werden soll.
* Mit QFileDialog wird der Name der Datei eingegeben.
*/
void FileManager::saving(void)
{
    if(!_isSaved)
    {
    _dirFilePath.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    _actualFile.setFileName(QFileDialog::getSaveFileName(nullptr, "Speichern", _dirFilePath.absolutePath(),
                                                         "Json (*.json);;Text (*.txt)"));
    }
    save();
    _isSaved = true;
}

/*!
* \brief Erzeugt eine neue Jsondatei, in der das Netzwerk abgespeichert wird.
*
* Es wird mit Hilfe von QStandardPaths der Fad angegeben, wo die Datei abgelegt werden soll.
* Mit QFileDialog wird der Name der Datei eingegeben.
* Darauf folgt das speichern.
*/
void FileManager::savingUnder(void)
{
    _dirFilePath.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    _actualFile.setFileName(QFileDialog::getSaveFileName(nullptr, "Speichern", _dirFilePath.absolutePath(),
                                                         "Json (*.json);;Text (*.txt)"));
    save();
    _isSaved = true;
}

//TODO: Doku fertigstellen in filemanager und überprüfen.
/*!
* \brief
*
*
*/
void FileManager::setProperties(QList<Component *> &components, QList<Connection *> &connections)
{
    _connections = &connections;
    _components = &components;
}

void FileManager::save(void)
{
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
    for (Component* c : *_components)
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

    for (Connection* c : *_connections)
    {
        array.append(saveConnection(c));
    }

    json.setArray(array);
    return json.toJson();
}

QJsonObject FileManager::saveResistor(Component* component)
{
    QJsonObject r;
    r.insert("type", "Resistor");
    r.insert("_name", component->getName());
    r.insert("_xPos", component->getXPosition());
    r.insert("_yPos", component->getYPosition());
    r.insert("_orientation", component->isVertical());
    r.insert("_resistance", component->getValue());
    return r;
}

QJsonObject FileManager::savePowerSupply(Component* component)
{
    QJsonObject ps;
    ps.insert("type", "PowerSupply");
    ps.insert("_name", component->getName());
    ps.insert("_xPos", component->getXPosition());
    ps.insert("_yPos", component->getYPosition());
    ps.insert("_orientation", component->isVertical());
    return ps;
}

QJsonObject FileManager::saveConnection(Connection* connection)
{
    QJsonObject c;
    c.insert("type", "Connection");
    c.insert("_componentPortOne", connection->getComponentPortOne().getComponent()->getName());
    c.insert("_componentPortTwo", connection->getComponentPortTwo().getComponent()->getName());
    c.insert("_portA", connection->getComponentPortOne().getPort());
    c.insert("_portB", connection->getComponentPortTwo().getPort());
    return c;
}

Component* FileManager::getComponentByName(QString name)
{
    for (Component* component : *_components)
    {
        if (component->getName() == name)
        {
            return component;
        }
    }
    return nullptr;
}

Component::Port FileManager::toPort(int port)
{
    if (port == 0)
    {
        return Component::Port::A;
    }
    else if (port == 1)
    {
        return Component::Port::B;
    }
    else
    {
        return Component::Port::null;
    }
}

/*!
* \brief Lädt ein Netzwerk aus einer vorhandenen Jsondatei.
*
* Es wird mit Hilfe von QStandardPaths der Fad angegeben, aus welcher Datei geladen werden soll.
* Mit QFileDialog wird .
* Darauf folgt ob die Datei geöffnet wurde und ob in dieser Daten enthalten sind.
* Zuerst werden die Components geladen.
* Dann folgen die Connectionss, da dies die Connections zugreifen.
*/
void FileManager::loading(void)
{
    _dirFilePath.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    _actualFile.setFileName(QFileDialog::getOpenFileName(nullptr, "Laden", _dirFilePath.absolutePath(), _fileFilter));
    _homePath.setPath(_homePath.filePath(_actualFile.fileName()));

    QJsonDocument json;
    QJsonArray array;
    QJsonArray res;

    if (_actualFile.open(QFile::ReadOnly))
    {
        const QByteArray file = _actualFile.readAll();
        json = QJsonDocument::fromJson(file, nullptr);

        if (!json.isEmpty() && json.isArray())
        {
            array = json.array();

            //Zuerst Components laden, da die Connections auf diese zugreifen / zeigen
            for (int i = 0; i < array.size(); i++)
            {
                if (array[i].isObject())
                {
                    QJsonObject obj = array[i].toObject();


                    if (obj.value("type") == "Resistor")
                    {
                        QString name = obj.value("_name").toString();
                        int xPos = obj.value("_xPos").toInt();
                        int yPos = obj.value("_yPos").toInt();
                        bool orientation = obj.value("_orientation").toBool();
                        int resistance = obj.value("_resistance").toInt();
                        Component* resistor = new Resistor(name, resistance, xPos, yPos, orientation);
                        _components->append(resistor);
                    }
                    else if (obj.value("type") == "PowerSupply")
                    {
                        QString name = obj.value("_name").toString();
                        int xPos = obj.value("_xPos").toInt();
                        int yPos = obj.value("_yPos").toInt();;
                        bool orientation = obj.value("_orientation").toBool();
                        Component* powerSupply = new PowerSupply(name, xPos, yPos, orientation);
                        _components->append(powerSupply);
                    }
                }
            }

            //Jetzt Connections zeichnen
            for (int i = 0; i < array.size(); i++)
            {
                if (array[i].isObject())
                {
                    QJsonObject obj = array[i].toObject();

                    if (obj.value("type") == "Connection")
                    {
                        QString nameA = obj.value("_componentPortOne").toString();
                        Component* componentA = getComponentByName(nameA);
                        qDebug() << componentA->getName();
                        Component::Port portA = toPort(obj.value("_portA").toInt());
                        ComponentPort componentPortA(componentA, portA);

                        QString nameB = obj.value("_componentPortTwo").toString();
                        Component* componentB = getComponentByName(nameB);
                        qDebug() << componentB->getName();
                        Component::Port portB = toPort(obj.value("_portB").toInt());
                        ComponentPort componentPortB(componentB, portB);

                        Connection* connection = new Connection(componentPortA, componentPortB);
                        _connections->append(connection);
                    }
                }
            }
        }
    }
    _actualFile.close();
    _isSaved = true;
}
