#ifndef EDITINGVIEW_H
#define EDITINGVIEW_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QWidget>

#include <model/resistor.h>
#include <model/powersupply.h>

class EditingView : public QWidget
{
    Q_OBJECT
public:
    explicit EditingView(Component* component, QWidget *parent = nullptr);

signals:

private:
    //Methoden
    void setupView(void);

    //Variablen
    QVBoxLayout* _vLayout = nullptr;
    QHBoxLayout* _hLayout = nullptr;
    QPushButton* _uebernehmenButton = nullptr;
    QLineEdit* _valueInput = nullptr;
    QLineEdit* _nameInput = nullptr;

    Component* _component = nullptr;
};

#endif // EDITINGVIEW_H
