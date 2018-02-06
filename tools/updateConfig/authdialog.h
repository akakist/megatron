#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>

namespace Ui {
    class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QWidget *parent = 0);
    ~AuthDialog();

signals:
    void do_auth(QString email,QString pincode);
private slots:
    void on_buttonBox_accepted();

private:
    Ui::AuthDialog *ui;
};

#endif // AUTHDIALOG_H
