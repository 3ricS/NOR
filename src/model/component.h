#ifndef COMPONENT_H
#define COMPONENT_H

#include <QObject>

class Component : public QObject
{
    Q_OBJECT
public:
    Component();

signals:
    void added(void);
    void remove(void);

private:
    int _xPosition;
    int _yPosition;
    int _id;
    static int _count;
};

#endif // COMPONENT_H
