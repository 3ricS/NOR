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
    enum MouseMode{ResistorMode, PowerSupplyMode, DeleteMode, Mouse};
    explicit Model(QObject *parent = nullptr);
    void clickInterpretation(QPointF position);
    void setMode(MouseMode newMode){_mode = newMode;}
    //getter
    QList<Component*> getComponentList() {return _componentList;}

signals:
    void modelChanged(void);

private:
    void addObject(Component* component);
    void addResistor(QString name, int value, int x, int y);
    void addPowerSupply(QString name, int x, int y);
    void tryFindComponent(QPointF position);

    //Variables
    MouseMode _mode = Mouse;
    QList<Component*> _componentList;

};

#endif // MODEL_H
