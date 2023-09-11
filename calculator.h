#ifndef CALCULATOR_H
#define CALCULATOR_H


#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include "QRadioButton"
#include "QLineEdit"

using namespace std;


class Calculator : public QWidget
{
    Q_OBJECT
public:
    Calculator(QWidget* parent = nullptr);

public slots:
    void create_standart_calc(QWidget*);
    void create_engin_calc(QWidget*);
    void switch_mode();
    void create_default_part(QWidget*);
    void clear_input();
    void equals();
    void binary_func();
    void unary_func();
    void dot_clicked();
    void num_clicked();

private:
    QWidget* standart_calc;
    QWidget* engin_calc;
    QWidget* default_part;
    QLineEdit* input_field;
    QLineEdit* operation_field;
    QRadioButton* standart_button;
    QRadioButton* engin_button;
    QFrame* frame;
    string cache;
    bool cached_operation = false;
    unsigned long long input_max_digits;
    bool clean = false;
    QString cur_operation;
};
#endif // CALCULATOR_H
