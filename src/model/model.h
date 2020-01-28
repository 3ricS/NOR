#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QDebug>

#include <model/component.h>
#include <model/resistor.h>
#include <model/powersupply.h>
#include <model/connection.h>

class Model : public QObject
{
    Q_OBJECT
public:
    enum MouseMode{ResistorMode, PowerSupplyMode, ConnectionMode, DeleteMode, Mouse};
    explicit Model(QObject *parent = nullptr);
    void clickInterpretation(QPointF position, QPointF positionEnd);
    void setMode(MouseMode newMode) {_mode = newMode;}
    //getter
    QList<Component*> getComponentList() {return _componentList;}

    //Test für Connection Mode
    MouseMode _mode = Mouse;

signals:
    void modelChanged(void);

private:
    void addObject(Component* component);
    void addResistor(QString name, int value, int x, int y);
    void addPowerSupply(QString name, int x, int y);
    void addConnection(int xStart, int yStart, int xEnd, int yEnd);
    void tryFindComponent(QPointF position);

    //Variables
    //MouseMode _mode = Mouse;
    QList<Component*> _componentList;

};

#endif // MODEL_H
