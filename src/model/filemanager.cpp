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

/*!
 * \brief Bekommt den Dateinamen.
 *
 * \return Gibt den Dateinamen zurück.
 *
 * Sucht nach dem Dateinamen der aktuell ausgewählten Datei.
 * Der Filepath wird beim Slash gesplittet und das letzte Element ist der eigentliche Name.
 */
QString FileManager::getFileName(void)
{
    QStringList list = _actualFile.fileName().split('/');
    return list.last();
}

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

    for (Connection* connection : _model->getConnections())
    {
        array.append(saveConnection(connection));
    }

    for (Description* description : _model->getDescriptions())
    {
        array.append(saveDescription(description));
    }

    json.setArray(array);
    return json.toJson();
}

QJsonObject FileManager::saveComponent(Component* component)
{
    QJsonObject c;
    c.insert("type", component->getComponentTypeInt());
    c.insert("id", component->getId());
    c.insert("name", component->getName());
    c.insert("xPos", component->getPosition().x());
    c.insert("yPos", component->getPosition().y());
    c.insert("isVertical", component->isVertical());
    Resistor* resistor = dynamic_cast<Resistor*>(component);
    bool isResistor = (nullptr != resistor);
    if (isResistor)
    {
        c.insert("value", static_cast<double>(resistor->getResistanceValue()));
    }
    else
    {
        c.insert("value", component->getVoltage());
    }

    return c;
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

QJsonObject FileManager::saveDescription(Description *description)
{
    QJsonObject df;
    df.insert("type", "Description");
    df.insert("id", description->getId());
    df.insert("xPos", description->getPosition().x());
    df.insert("yPos", description->getPosition().y());
    df.insert("text", description->getText());
    return df;
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
            loadComponent(array);
            //Jetzt Connections zeichnen
            loadConnection(array);
            //Jetzt die Textfelder Laden
            loadDescription(array);
        }
    }
    _actualFile.close();
    _isSaved = true;
}

void FileManager::loadComponent(QJsonArray array)
{
    for (int i = 0; i < array.size(); i++)
    {
        if (array[i].isObject())
        {
            QJsonObject obj = array[i].toObject();
            if(obj.value("type") == 0 || obj.value("type") == 1)
            {
                QString name = obj.value("name").toString();
                int id = obj.value("id").toInt();
                int xPos = obj.value("xPos").toInt();
                int yPos = obj.value("yPos").toInt();
                bool isVertical = obj.value("isVertical").toBool();
                long double value = obj.value("value").toDouble();
                Component::ComponentType componentType = Component::convertToComponentType(obj.value("type").toInt());
                if(Component::ComponentType::Resistor == componentType)
                {
                    _model->addResistor(name, value, xPos, yPos, isVertical, id);
                }
                else if(Component::ComponentType::PowerSupply == componentType)
                {
                    _model->addPowerSupply(name, xPos, yPos, isVertical, value, id);
                }
            }
        }
    }
}

void FileManager::loadConnection(QJsonArray array)
{
    for (int i = 0; i < array.size(); i++)
    {
        if (array[i].isObject())
        {
            QJsonObject obj = array[i].toObject();

            if (obj.value("type") == "Connection")
            {
                int idA = obj.value("componentIdOne").toInt();
                int portIntA = obj.value("portA").toInt();
                Component* componentA = _model->getComponentById(idA);
                Component::Port portA = toPort(portIntA);
                ComponentPort componentPortA(componentA, portA, _model);

                int idB = obj.value("componentIdTwo").toInt();
                int portIntB = obj.value("portB").toInt();
                Component* componentB = _model->getComponentById(idB);
                Component::Port portB = toPort(portIntB);
                ComponentPort componentPortB(componentB, portB, _model);

                _model->addConnectionWithoutUndo(componentPortA, componentPortB);
            }
        }
    }
}

void FileManager::loadDescription(QJsonArray array)
{
    for (int i = 0; i < array.size(); i++)
    {
        if (array[i].isObject())
        {
            QJsonObject obj = array[i].toObject();

            if (obj.value("type") == "Description")
            {
                int id = obj.value("id").toInt();
                int xPos = obj.value("xPos").toInt();
                int yPos = obj.value("yPos").toInt();
                QString text = obj.value("text").toString();

                _model->addDescriptionWithoutUndo(QPointF(xPos, yPos), true, text, id);
            }
        }
    }
}
