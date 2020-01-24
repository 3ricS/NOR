#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include <model/component.h>

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);
    void paintObject(void);

signals:
    void newResistorElement(void);

private:
    QList<Component> _componentList;       //statt int wird es eine Liste von Widerständen sein?

};

#endif // MODEL_H
