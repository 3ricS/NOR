#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <QString>

#include <model/component.h>

class PowerSupply : public Component
{
public:
    PowerSupply(QString name, int x, int y);

    //getter
    virtual int getXStartPosition() override;
    virtual int getYStartPosition() override;

    int getWidth() {return _width;}
    int getHeight() {return _height;}


// Component interface
signals:
    void draw();
    void remove();

private:
    QString _name;
    static constexpr int _height = 120;
    static constexpr int _width = 60;

};

#endif // POWERSUPPLY_H
