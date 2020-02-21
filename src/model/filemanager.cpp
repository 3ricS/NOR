#include "filemanager.h"

FileManager::FileManager(NetworkGraphics *model) :
    _model(model)
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
void FileManager::save(void)
{
    if(!_isSaved)
    {
        _dirFilePath.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        _actualFile.setFileName(QFileDialog::getSaveFileName(nullptr, "Speichern", _dirFilePath.absolutePath() + _defaultFileName,
                                                             "Json (*.json);;Text (*.txt)"));
    }
    _isSaved = saveData();
}

/*!
* \brief Erzeugt eine neue Jsondatei, in der das Netzwerk abgespeichert wird.
*
* Es wird mit Hilfe von QStandardPaths der Fad angegeben, wo die Datei abgelegt werden soll.
* Mit QFileDialog wird der Name der Datei eingegeben.
* Darauf folgt das speichern.
*/
void FileManager::saveAs(void)
{
    _dirFilePath.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    _actualFile.setFileName(QFileDialog::getSaveFileName(nullptr, "Speichern",_dirFilePath.absolutePath() + _defaultFileName,
                                                         "Json (*.json);;Text (*.txt)"));
    _isSaved = saveData();
}

//Filepath wird gesplittet beim Slash und das letzte Element ist der eigentliche Name
QString FileManager::getFileName(void)
{
  QStringList list = _actualFile.fileName().split('/');
  return list.last();
}

//TODO: Doku fertigstellen in filemanager und überprüfen.

bool FileManager::saveData(void)
{
    if (_actualFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&_actualFile);
        out << createJson();
        _actualFile.close();
        return true;
    }
    return false;
}

QString FileManager::createJson(void)
{
    QJsonDocument json;
    QJsonArray array;
    for (Component* component : _model->getComponents())
    {
        array.append(saveComponent(component));
    }

    for (Connection* c : _model->getConnections())
    {
        array.append(saveConnection(c));
    }

    for (DescriptionField* description : _model->getDescriptions())
    {
        array.append(saveDescription(description));
    }

    json.setArray(array);
    return json.toJson();
}

QJsonObject FileManager::saveComponent(Component* component)
{
    QJsonObject r;
    r.insert("type", component->getComponentType());
    r.insert("id", component->getId());
    r.insert("name", component->getName());
    r.insert("xPos", component->getXPosition());
    r.insert("yPos", component->getYPosition());
    r.insert("isVertical", component->isVertical());
    r.insert("resistance", component->getValue());
    return r;
}

QJsonObject FileManager::saveConnection(Connection* connection)
{
    QJsonObject c;
    c.insert("type", "Connection");
    c.insert("componentIdOne", connection->getComponentPortOne().getComponent()->getId());
    c.insert("componentIdTwo", connection->getComponentPortTwo().getComponent()->getId());
    c.insert("portA", connection->getComponentPortOne().getPort());
    c.insert("portB", connection->getComponentPortTwo().getPort());
    return c;
}

QJsonObject FileManager::saveDescription(DescriptionField *description)
{
    QJsonObject df;
    df.insert("type", "DescriptionField");
    df.insert("id", description->getId());
    df.insert("xPos", description->getXPos());
    df.insert("yPos", description->getYPos());
    df.insert("text", description->getText());
    return df;
}

Component* FileManager::getComponentById(int id)
{
    for (Component* component : _model->getComponents())
    {
        if (component->getId() == id)
        {
            return component;
        }
    }
    return nullptr;
}

Component::Port FileManager::toPort(int componentPort)
{
    Component::Port port = Component::null;
    if(0 == componentPort)
    {
        port = Component::Port::A;
    }
    else if (1 == componentPort)
    {
        port = Component::Port::B;
    }
    else if (2 == componentPort) {
        port = Component::Port::null;
    }
    return port;
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
void FileManager::load(void)
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
                    QString name = obj.value("name").toString();
                    int id = obj.value("id").toInt();
                    int xPos = obj.value("xPos").toInt();
                    int yPos = obj.value("yPos").toInt();
                    bool isVertical = obj.value("isVertical").toBool();
                    int resistance = obj.value("resistance").toInt();
                    Component::ComponentType componentType = Component::integerToComponentType(obj.value("type").toInt());
                    if(Component::ComponentType::Resistor == componentType)
                    {
                        _model->addResistor(name, resistance, xPos, yPos, isVertical, id);
                    }
                    else if(Component::ComponentType::PowerSupply == componentType)
                    {
                        _model->addPowerSupply(name, xPos, yPos, isVertical, id);
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
                        int idA = obj.value("componentIdOne").toInt();
                        Component* componentA = getComponentById(idA);
                        Component::Port portA = toPort(obj.value("portA").toInt());
                        ComponentPort componentPortA(componentA, portA);

                        int idB = obj.value("componentIdTwo").toInt();
                        Component* componentB = getComponentById(idB);
                        Component::Port portB = toPort(obj.value("portB").toInt());
                        ComponentPort componentPortB(componentB, portB);

                        _model->addConnection(componentPortA, componentPortB);
                    }
                }
            }
        }
    }
    _actualFile.close();
    _isSaved = true;
}
