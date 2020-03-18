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

class Component : public QGraphicsItem
{
public:
    enum ComponentType{Resistor, PowerSupply};
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
    int getXPosition(void) const {return _xPosition;}
    int getYPosition(void) const {return _yPosition;}
    QString getName(void) const {return _name;}
    bool isVertical(void) const {return _isVertical;}
    bool isSelected(void) const {return  _isSelected;}
    double getVoltage(void) {return _voltage;}
    double getAmp(void) {return _amp;}

    Component::Port getPort(QPointF position) const;
    QPointF getPortPosition(Port port) const;
    int getId(void) {return _id;}
    Orientation getOrientation(void) {return _orientation;}
    QPointF getPosition(void) {return QPointF(_xPosition, _yPosition);}

    //setter
    void setVertical(bool orientation) {_isVertical = orientation;}
    void setOrientation(Component::Orientation newOrientation);
    void setName(QString name) {_name = name;}
    void setPosition(QPointF gridPosition);
    void setSelected(bool isSelected);
    void setVoltage(double voltage) {_voltage = voltage;}
    void setAmp(double amp) {_amp = amp;}

    static constexpr int _hitBoxSize = 20;

protected:
    //Variables
    int  _xPosition;
    int  _yPosition;
    bool _isVertical;
    bool _isSelected = false;
    Orientation _orientation;

    QString     _name;

    double _voltage = 0.0;
    double _amp = 0.0;

    //virtuelle Methoden
    virtual void paintInformation(QPainter* painter) = 0;

private:
    int getPortPositionXOrY(int positionValue, Port port, bool isX) const;

    const int _id;

    //_componentTyp gets the type of the object
    const ComponentType _componentType;
};

#endif // COMPONENT_H
