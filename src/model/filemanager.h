#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QStandardPaths>

#include <model/connection.h>
#include <model/powersupply.h>
#include <model/resistor.h>

class FileManager
{
public:
    FileManager(QList<Component *> &components, QList<Connection *> &connections);

    void saving(void);
    void loading(void);

private:
    QString createSaveData(void);
    QJsonObject saveResistor(Component* component);
    QJsonObject savePowerSupply(Component* component);
    QJsonObject saveConnection(Connection* connection);
    Component* getComponentByName(QString name);
    Component::Port toPort(int port);

    QFile _actualFile;
    QString  _fileFilter  = "Json (*.json);;Text (*.txt)";
    QDir     _homePath;

    QList<Component*>* _components;
    QList<Connection*>* _connections;
    QDir _dirFilePath;


};

#endif // FILEMANAGER_H
