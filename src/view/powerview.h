#ifndef POWERVIEW_H
#define POWERVIEW_H

#include <QDialog>

namespace Ui {
class PowerView;
}

class PowerView : public QDialog
{
    Q_OBJECT

public:
    explicit PowerView(QWidget *parent = nullptr);
    ~PowerView();

    void setText(QString text);

private:
    Ui::PowerView *ui;
};

#endif // POWERVIEW_H
