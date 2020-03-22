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

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#include "model/gridobject.h"

class Component : public GridObject
{
public:
    enum ComponentType{Resistor, PowerSupply, Null};
    enum Port{A, B, null};
    enum Orientation{left, top, right, bottom};
    Component(QPointF position, bool isVertical, QString name, double voltage, ComponentType componentTyp,
              int id);

    bool operator==(const Component& rhs);
    bool operator!=(const Component& rhs);

    bool    hasPortAtPosition(QPointF position) const;
    static  ComponentType convertToComponentType(int componentType);



    //getter
    bool            isVertical() const;
    int             getComponentTypeInt(void) const {return _componentType;}
    ComponentType   getComponentType(void) const {return _componentType;}
    QString         getName(void) const {return _name;}
    double          getVoltage(void) {return _voltage;}
    double          getAmp(void) {return _amp;}

    Component::Port getPort(QPointF position) const;
    QPointF         getPortPosition(Port port) const;
    Orientation     getOrientation(void) {return _orientation;}

    //setter
    void setOrientation(Component::Orientation newOrientation) {_orientation = newOrientation;}
    void setName(QString name) {_name = name;}
    void setVoltage(double voltage) {_voltage = voltage;}
    void setAmp(double amp) {_amp = amp;}

    static constexpr int _hitBoxSizeAtPort = 20;

protected:
    virtual void    setLabelPositions(QPainter* painter) = 0;
    void            paintInformation(QPainter* painter, QString name, double value, QRectF namePosition, QRectF valuePosition,
                                     ComponentType componentType);

    QString     _name;
    Orientation _orientation;
    double _voltage = 0.0;
    double _amp = 0.0;

private:
    int     getPortPositionXOrY(int positionValue, Port port, bool isX) const;
    QString getDisplayedValueString(double value, ComponentType componentType);
    QString getUnitAndFitValue(double& valueWithoutUnit);

    const ComponentType _componentType;
};

#endif // COMPONENT_H
