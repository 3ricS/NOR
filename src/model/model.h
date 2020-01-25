#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QDebug>

#include <model/component.h>
#include <model/resistor.h>
#include <model/powersupply.h>

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);
    void addResistor(QString name, int value, int x, int y);
    void addPowerSupply(QString name, int x, int y);

    //getter
    QList<Component*> getComponentList() {return _componentList;}

signals:
    void modelChanged(void);

private:
    void addObject(Component* component);

    QList<Component*> _componentList;

};

#endif // MODEL_H
