#include "powerview.h"
#include "ui_powerview.h"

#include <QPushButton>

PowerView::PowerView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerView)
{
    ui->setupUi(this);
    setWindowTitle("Ströme und Spannungen");
    ui->buttonBox->button(QDialogButtonBox::Close)->setText("Schließen");
}

PowerView::~PowerView(void)
{
    delete ui;
}

/*!
 * \brief Initialisiert den dargestellten Text.
 *
 * \param text ist der darzustellende Text
 *
 * Der darzustellende Text wird in das Label geschrieben.
 */
void PowerView::setText(QString text)
{
    ui->label->setText(text);
}
