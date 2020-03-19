/*!
* \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
*
* \brief Übernimmt das Laden und Speichern von Netzwerken in Datein
*
*/
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QStandardPaths>
#include <QTextStream>

#include <model/networkgraphics.h>
#include <model/connection.h>
#include <model/powersupply.h>
#include <model/resistor.h>
#include <model/description.h>

class NetworkGraphics;

class FileManager
{
public:
    FileManager(NetworkGraphics* model);

    void save(void);
    void load(void);
    void saveAs(void);

    //Getter
    QString getFileName(void);

private:
    bool saveData(void);
    QString createJson(void);

    QJsonObject saveComponent(Component* component);
    QJsonObject saveConnection(Connection* connection);
    QJsonObject saveDescription(Description* description);
    void loadComponent(QJsonArray array);
    void loadConnection(QJsonArray array);
    void loadDescription(QJsonArray array);

    Component::Port toPort(int componentPort);

    QFile    _actualFile;
    const QString  _fileFilter  = "Json (*.json);;Text (*.txt)";
    QDir     _homePath;
    bool     _isSaved = false;

    NetworkGraphics* _model = nullptr;
    QDir             _dirFilePath;
    const QString    _defaultFileName = "/Widerstandsnetzwerk";


};

#endif // FILEMANAGER_H
