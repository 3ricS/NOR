#ifndef EDITINGVIEW_H
#define EDITINGVIEW_H

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QComboBox>

#include <model/resistor.h>
#include <model/powersupply.h>
#include <ui_editview.h>


namespace Ui {
    class EditView;
}

class EditView : public QDialog
{
    Q_OBJECT
public:
    explicit EditView(Component* component, QWidget *parent = nullptr);

signals:

private:
    //Methoden
    void setupView(void);
    virtual void accept(void) override;

    //Variablen
    Ui::EditView* _editViewUi;
    QComboBox* _orientationBox = nullptr;
    QLabel* _orientationTitle = nullptr;
    Component* _component = nullptr;
};

#endif // EDITINGVIEW_H
