#include "calculator.h"
#include <iostream>
#include <math.h>
#include <exception>
#include <sstream>


Calculator::Calculator(QWidget* parent)
    : QWidget(parent)

{
    setFixedSize(450, 550);
    QVBoxLayout* poLayout = new QVBoxLayout;
    poLayout->setSpacing(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(poLayout);
    setObjectName("main");
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    poLayout->addWidget(frame);
    frame->setGeometry(0, 0, 420, 520);


    default_part = new QWidget(frame);
    create_default_part(default_part);
    default_part->setGeometry(0, 0, 420, 210);

    standart_calc = new QWidget(frame);
    standart_calc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    create_standart_calc(standart_calc);
    standart_calc->setGeometry(0, 200, 420, 300);

    engin_calc = new QWidget(frame);
    engin_calc->setObjectName("enginCalc");
    create_engin_calc(engin_calc);
    engin_calc->setGeometry(0, 200, 336, 300);
    engin_calc->hide();




    setWindowTitle("Calculator(standart)");

    setStyleSheet(R"MYSTR(QWidget#main {background-color: rgb(230, 230, 250);}
                        QLineEdit {background-color: rgb(240,240,250); border: none; color: rgb(31, 32, 36)}
                        QRadioButton {color: rgb(38,40,47); font-size: 13px}
                        QPushButton {background-color: rgb(57,59,64); color: rgb(230,230,230); font-size: 18px; border-radius:4px}
                        QPushButton:hover {background-color: rgb(49,50,56);}
                        QPushButton:pressed {background-color: rgb(38,40,47);}
                        QPushButton#push_button_equals {background-color: rgb(164, 31, 230); color: rgb(230,230,230)}
                        QPushButton#push_button_equals:hover {background-color: rgb(144, 28, 202);}
                        QPushButton#push_button_equals:pressed {background-color: rgb(130, 25, 182);}
                        QLineEdit#operationField {color: rgb(160,161,164)})MYSTR");
}

string to_scien(double number) {
    ostringstream str;
    str << scientific << number;
    return str.str();
}


void Calculator::switch_mode() {
    if (standart_button->isChecked()) {
        input_max_digits = 15;
        frame->setGeometry(0, 0, 430, 520);
        setFixedSize(450, 520);
        default_part->setGeometry(0, 0, 420, 210);
        standart_calc->setGeometry(0, 200, 420, 300);
        engin_calc->hide();
        setWindowTitle("Calculator(standart)");
    }
    else {
        frame->setGeometry(0, 0, 770, 520);
        setFixedSize(770, 520);
        default_part->setGeometry(0, 0, 750, 210);
        standart_calc->setGeometry(330, 200, 420, 300);
        engin_calc->show();
        input_field->setMaxLength(30);
        input_max_digits = 30;
        setWindowTitle("Calculator(engineering)");
    }
}

void Calculator::num_clicked() {
    if (clean) {
        input_field->clear();
        clean = false;
    }
    if (input_field->text() == "0") {
        input_field->clear();
    }
    QPushButton* button = qobject_cast<QPushButton*> (sender());
    if (!button)
        return;
    QString number = button->text();
    input_field->setText((input_field->text() + number));
}


void Calculator::dot_clicked() {
    if (clean) {
        input_field->setText("0.");
    }
    else if (!input_field->text().contains(".")) {
        if (input_field->text().length()) {
            input_field->setText(input_field->text() + '.');
        }
        else {
            input_field->setText("0.");
        }
    }
}


void Calculator::unary_func() {
    QPushButton* button = qobject_cast<QPushButton*> (sender());
    if (!button)
        return;
    cur_operation = button->text();
    string expression = input_field->text().toStdString();
    double number;
    if (expression == "") {
        return;
    }
    if (expression == "\u03C0") {
        number = 3.1415926535;
    }
    else if (expression == "e") {
        number = 2.718281828;
    }
    else
        number = stod(expression);
    string operation = cur_operation.toStdString() + "(" + expression + ")";
    if (cur_operation == "\u221A")
        number = sqrt(number);
    else if (cur_operation == "sin")
        number = sin(number);
    else if (cur_operation == "sinh")
        number = sinh(number);
    else if (cur_operation == "cos")
        number = cos(number);
    else if (cur_operation == "cosh")
        number = cosh(number);
    else if (cur_operation == "tan")
        number = tan(number);
    else if (cur_operation == "tanh")
        number = tanh(number);
    else if (cur_operation == "log")
        number = log10(number);
    else if (cur_operation == "ln")
        number = log(number);
    else if (cur_operation == "x\u00B2") // x^2
    {
        operation = expression + "\u00B2";
        number *= number;
    }
    else if (cur_operation == "1/x") {
        operation = "1/(" + expression + ")";
        number = 1 / number;
    }
    operation += "=";
    expression = to_string(number);
    while ((*(expression.rbegin()) == '0') and (expression != "0")) {
        expression.pop_back();
    }
    if (*(expression.rbegin()) == '.')
        expression.pop_back();
    if (expression.length() > input_max_digits)
        expression = to_scien(number);
    input_field->setText(expression.c_str());
    operation_field->setText(operation.c_str());
}

void Calculator::binary_func()
{
    if (input_field->text() != "")
    {
        cached_operation = true;
        clean = true;

        cache = input_field->text().toStdString();
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if (!button)
            return;
        cur_operation = button->text();
        std::string operationString;
        if (cur_operation == "\u207F\u221A")
            operationString = cache + "^(1/";
        else if (cur_operation == "x\u207F")
            operationString = cache + "^";
        else
            operationString = cache + cur_operation.toStdString();
        operation_field->setText(tr(operationString.c_str()));
    }
}


void Calculator::equals()
{
    if (cached_operation)
    {
        std::string operationString;
        double operand1;
        double operand2;
        if (cache == "\u03C0") {
            operand1 = 3.1415926535;
        }
        else if (cache == "e") {
            operand1 = 2.718281828;
        }
        else
            operand1 = stod(cache);
        if (input_field->text() == "\u03C0") {
            operand2 = 3.1415926535;
        }
        else if (input_field->text() == "e") {
            operand2 = 2.718281828;
        }
        else
            operand2 = stod(input_field->text().toStdString());
        operationString = operation_field->text().toStdString() + input_field->text().toStdString();
        if (cur_operation == "+")
            operand1 += operand2;
        else if (cur_operation == "-")
            operand1 -= operand2;
        else if (cur_operation == "*")
            operand1 *= operand2;
        else if (cur_operation == "/")
            operand1 /= operand2;
        else if (cur_operation == "x\u207F") // x^n
            operand1 = pow(operand1, operand2);
        else if (cur_operation == "\u207F\u221A") // sqrt^n
        {
            operationString += ")";
            operand1 = pow(operand1, 1 / operand2);
        }
        cache = to_string(operand1);
        while ((*(cache.rbegin()) == '0') and (cache != "0"))
            cache.pop_back();
        if (*(cache.rbegin()) == '.')
            cache.pop_back();
        operationString += "=";
        operation_field->setText(tr(operationString.c_str()));
        input_field->setText(tr(cache.c_str()));
        cached_operation = false;
    }
}


void Calculator::clear_input()
{
    operation_field->clear();
    input_field->setText("0");
}

void Calculator::create_default_part(QWidget* default_part) {
    auto my_layout = new QGridLayout(default_part);
    input_field = new QLineEdit("0");
    operation_field = new QLineEdit();
    standart_button = new QRadioButton("Standart");
    engin_button = new QRadioButton("Engineering");


    my_layout->addWidget(input_field, 1, 0, 1, 1);
    my_layout->addWidget(operation_field, 0, 0, 1, 1);
    my_layout->addWidget(standart_button, 2, 0, 1, 1);
    my_layout->addWidget(engin_button, 3, 0, 1, 1);


    input_field->setFixedHeight(70);
    operation_field->setFixedHeight(30);


    standart_button->setChecked(true);
    input_field->setAlignment(Qt::AlignRight);
    operation_field->setAlignment(Qt::AlignRight);
    QFont font;
    font.setPointSize(30);
    input_field->setFont(font);
    input_field->setMaxLength(15);
    input_max_digits = 15;
    input_field->setReadOnly(true);

    font.setPointSize(15);
    operation_field->setFont(font);
    operation_field->setReadOnly(true);

    connect(standart_button, SIGNAL(toggled(bool)), this, SLOT(switch_mode()));

}


void Calculator::create_standart_calc(QWidget* standart_calc) {
    auto my_layout = new QGridLayout(standart_calc);
    QPushButton* button_c = new QPushButton("C");
    button_c->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_0 = new QPushButton("0");
    button_0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_1 = new QPushButton("1");
    button_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_2 = new QPushButton("2");
    button_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_3 = new QPushButton("3");
    button_3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_4 = new QPushButton("4");
    button_4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_5 = new QPushButton("5");
    button_5->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_6 = new QPushButton("6");
    button_6->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_7 = new QPushButton("7");
    button_7->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_8 = new QPushButton("8");
    button_8->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_9 = new QPushButton("9");
    button_9->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_dot = new QPushButton(".");
    button_dot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_minus = new QPushButton("-");
    button_minus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_plus = new QPushButton("+");
    button_plus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_mult = new QPushButton("*");
    button_mult->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_div = new QPushButton("/");
    button_div->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_equal = new QPushButton("=");
    button_equal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_equal->setObjectName("push_button_equals");
    QPushButton* button_sqrt = new QPushButton(QString::fromUtf8("\u221A"));
    button_sqrt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_frac = new QPushButton("1/x");
    button_frac->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_square = new QPushButton(QString::fromUtf8("x\u00B2"));
    button_square->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    my_layout->addWidget(button_0, 3, 1, 1, 1);
    my_layout->addWidget(button_1, 2, 0, 1, 1);
    my_layout->addWidget(button_2, 2, 1, 1, 1);
    my_layout->addWidget(button_3, 2, 2, 1, 1);
    my_layout->addWidget(button_4, 1, 0, 1, 1);
    my_layout->addWidget(button_5, 1, 1, 1, 1);
    my_layout->addWidget(button_6, 1, 2, 1, 1);
    my_layout->addWidget(button_7, 0, 0, 1, 1);
    my_layout->addWidget(button_8, 0, 1, 1, 1);
    my_layout->addWidget(button_9, 0, 2, 1, 1);
    my_layout->addWidget(button_dot, 3, 2, 1, 1);
    my_layout->addWidget(button_plus, 0, 3, 1, 1);
    my_layout->addWidget(button_c, 3, 0, 1, 1);
    my_layout->addWidget(button_mult, 1, 3, 1, 1);
    my_layout->addWidget(button_div, 1, 4, 1, 1);
    my_layout->addWidget(button_equal, 3, 4, 1, 1);
    my_layout->addWidget(button_sqrt, 2, 3, 1, 1);
    my_layout->addWidget(button_frac, 3, 3, 1, 1);
    my_layout->addWidget(button_minus, 0, 4, 1, 1);
    my_layout->addWidget(button_square, 2, 4, 1, 1);

    connect(button_0, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_1, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_2, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_3, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_4, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_5, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_6, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_7, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_8, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_9, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_dot, SIGNAL(clicked()), this, SLOT(dot_clicked()));
    connect(button_plus, SIGNAL(clicked()), this, SLOT(binary_func()));
    connect(button_minus, SIGNAL(clicked()), this, SLOT(binary_func()));
    connect(button_mult, SIGNAL(clicked()), this, SLOT(binary_func()));
    connect(button_div, SIGNAL(clicked()), this, SLOT(binary_func()));
    connect(button_equal, SIGNAL(clicked()), this, SLOT(equals()));
    connect(button_sqrt, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_frac, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_c, SIGNAL(clicked()), this, SLOT(clear_input()));
    connect(button_square, SIGNAL(clicked()), this, SLOT(unary_func()));
}


void Calculator::create_engin_calc(QWidget* engin_calc) {
    auto my_layout = new QGridLayout(engin_calc);

    QPushButton* button_sinh = new QPushButton(("sinh"), engin_calc);
    button_sinh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_sin = new QPushButton(("sin"), engin_calc);
    button_sin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_exp = new QPushButton(("e"), engin_calc);
    button_exp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_cosh = new QPushButton(("cosh"), engin_calc);
    button_cosh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_cos = new QPushButton(("cos"), engin_calc);
    button_cos->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_ln = new QPushButton(("ln"), engin_calc);
    button_ln->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_tanh = new QPushButton(("tanh"), engin_calc);
    button_tanh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_tan = new QPushButton(("tan"), engin_calc);
    button_tan->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_log = new QPushButton(("log"), engin_calc);
    button_log->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_xn = new QPushButton(QString::fromUtf8("x\u207F"), engin_calc); // x^n
    button_xn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_pi = new QPushButton(QString::fromUtf8("\u03C0"), engin_calc); // pi
    button_pi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPushButton* button_root = new QPushButton(QString::fromUtf8("\u207F\u221A"), engin_calc); // sqrt^n
    button_root->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    my_layout->addWidget(button_sinh, 1, 0, 1, 1);
    my_layout->addWidget(button_sin, 0, 0, 1, 1);
    my_layout->addWidget(button_exp, 3, 0, 1, 1);
    my_layout->addWidget(button_cosh, 1, 1, 1, 1);
    my_layout->addWidget(button_cos, 0, 1, 1, 1);
    my_layout->addWidget(button_ln, 2, 1, 1, 1);
    my_layout->addWidget(button_tanh, 1, 2, 1, 1);
    my_layout->addWidget(button_tan, 0, 2, 1, 1);
    my_layout->addWidget(button_log, 3, 1, 1, 1);
    my_layout->addWidget(button_xn, 3, 2, 1, 1);
    my_layout->addWidget(button_pi, 2, 0, 1, 1);
    my_layout->addWidget(button_root, 2, 2, 1, 1);



    connect(button_sin, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_sinh, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_cos, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_cosh, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_tan, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_tanh, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_ln, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_log, SIGNAL(clicked()), this, SLOT(unary_func()));
    connect(button_xn, SIGNAL(clicked()), this, SLOT(binary_func()));
    connect(button_root, SIGNAL(clicked()), this, SLOT(binary_func()));
    connect(button_exp, SIGNAL(clicked()), this, SLOT(num_clicked()));
    connect(button_pi, SIGNAL(clicked()), this, SLOT(num_clicked()));
}

