/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein Component stellt eine Komponente in der Schaltung dar
 *
 * Ein Component ist eine Komponente der Schaltung, z.B. ein Widerstand oder eine Spannungsquelle.
 * Ein Component ist ein QGraphicsItem, damit ein Element der QGraphicsScene und kann sich selbst in die QGraphicsScene
 * zeichnen.
 * Ein Component hat zwei Ports (Anschlüsse). Außerdem hat ein Component einen Wert und kennt seinen eigenen Typen (z.B. Widerstand oder Spannungsquelle).
 * Darüber hinaus hat ein Component eine Position und einen Namen.
 */
#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "gridobject.h"

class Component : public GridObject
{
public:
    enum ComponentType{Resistor, PowerSupply, Null};
    enum Port{A, B, null};
    enum Orientation{left, top, right, bottom};
    Component(int x, int y, bool isVertical, QString name, double voltage, ComponentType componentTyp, int id);

    QRectF boundingRect(void) const;
    bool hasPortAtPosition(QPointF position) const;
    static ComponentType integerToComponentType(int componentType);

    void paintHighlightRect(QPainter* painter);

    //getter
    int getComponentTypeInt(void) const {return _componentType;}
    ComponentType getComponentType(void) const {return _componentType;}
    QString getName(void) const {return _name;}
    bool isVertical(void) const {return _isVertical;}
    double getVoltage(void) {return _voltage;}
    double getAmp(void) {return _amp;}

    Component::Port getPort(QPointF position) const;
    QPointF getPortPosition(Port port) const;
    Orientation getOrientation(void) {return _orientation;}

    //setter
    void setVertical(bool orientation) {_isVertical = orientation;}
    void setOrientation(Component::Orientation newOrientation);
    void setName(QString name) {_name = name;}
    void setVoltage(double voltage) {_voltage = voltage;}
    void setAmp(double amp) {_amp = amp;}

    static constexpr int _hitBoxSize = 20;

protected:
    //Variables
    //TODO: _isVertical entfernen
    bool _isVertical;
    Orientation _orientation;

    QString     _name;

    double _voltage = 0.0;
    double _amp = 0.0;

    //virtuelle Methoden
    virtual void setLabelPositions(QPainter* painter) = 0;
    void paintInformation(QPainter* painter, QString name, double value, QRectF namePosition, QRectF valuePosition,
                          ComponentType componentType);

private:
    int getPortPositionXOrY(int positionValue, Port port, bool isX) const;
    QString getDisplayedValueString(double value, ComponentType componentType);
    QString getScaledValue(double
    & valueWithoutUnit);

    //_componentTyp gets the type of the object
    const ComponentType _componentType;
};

#endif // COMPONENT_H
