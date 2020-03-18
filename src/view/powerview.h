/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Der PowerView ist die Oberfläche, auf der die Ströme angezeigt.
 *
 * Es wird dargestellt:
 *      Der Spannungswert, der Spannungsquelle
 *      Der Gesamtstrom
 *      Die Spannungen, die an den Widerständen abfallen
 *      Der Strom, der an jedem Widerstand fließt
 */
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
    ~PowerView(void);

    void setText(QString text);

private:
    Ui::PowerView *ui;
};

#endif // POWERVIEW_H
