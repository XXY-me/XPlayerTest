#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_XPlayerTest.h"

class XPlayerTest : public QMainWindow
{
    Q_OBJECT

public:
    XPlayerTest(QWidget *parent = Q_NULLPTR);

private:
    Ui::XPlayerTestClass ui;
};
