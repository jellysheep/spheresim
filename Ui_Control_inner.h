/********************************************************************************
** Form generated from reading UI file 'Calculationsik5186.ui'
**
** Created: Tue Feb 12 19:25:17 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef CALCULATIONSIK5186_H
#define CALCULATIONSIK5186_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Calculations
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QDoubleSpinBox *radius_max;
    QDoubleSpinBox *radius_min;
    QLabel *label_6;
    QLabel *label_18;
    QLabel *label_17;
    QCheckBox *one_size;
    QLabel *label_5;
    QSpinBox *count;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_7;
    QDoubleSpinBox *x;
    QLabel *label_10;
    QLabel *label_11;
    QDoubleSpinBox *y;
    QDoubleSpinBox *z;
    QLabel *label_12;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_5;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_19;
    QDoubleSpinBox *inter_gravity;
    QLabel *label_14;
    QCheckBox *wall_resistance;
    QLabel *label_20;
    QDoubleSpinBox *earth_gravity;
    QLabel *label_13;
    QDoubleSpinBox *e_modul;
    QDoubleSpinBox *air_resistance;
    QDoubleSpinBox *poissons_ratio;
    QLabel *label_21;
    QDoubleSpinBox *elasticity;

    void setupUi(QDockWidget *Calculations)
    {
        if (Calculations->objectName().isEmpty())
            Calculations->setObjectName(QString::fromUtf8("Calculations"));
        Calculations->resize(240, 508);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 133));
        groupBox_2->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        radius_max = new QDoubleSpinBox(groupBox_2);
        radius_max->setObjectName(QString::fromUtf8("radius_max"));
        radius_max->setEnabled(false);
        radius_max->setWrapping(false);
        radius_max->setAccelerated(true);
        radius_max->setDecimals(3);
        radius_max->setMinimum(0.001);
        radius_max->setMaximum(0.1);
        radius_max->setSingleStep(0.001);
        radius_max->setValue(0.01);

        gridLayout_4->addWidget(radius_max, 3, 1, 1, 1);

        radius_min = new QDoubleSpinBox(groupBox_2);
        radius_min->setObjectName(QString::fromUtf8("radius_min"));
        radius_min->setWrapping(false);
        radius_min->setAccelerated(true);
        radius_min->setDecimals(3);
        radius_min->setMinimum(0.001);
        radius_min->setMaximum(0.1);
        radius_min->setSingleStep(0.001);
        radius_min->setValue(0.01);

        gridLayout_4->addWidget(radius_min, 1, 1, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_4->addWidget(label_6, 0, 0, 1, 1);

        label_18 = new QLabel(groupBox_2);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout_4->addWidget(label_18, 3, 0, 1, 1);

        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout_4->addWidget(label_17, 2, 0, 1, 1);

        one_size = new QCheckBox(groupBox_2);
        one_size->setObjectName(QString::fromUtf8("one_size"));
        one_size->setChecked(true);

        gridLayout_4->addWidget(one_size, 2, 1, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_4->addWidget(label_5, 1, 0, 1, 1);

        count = new QSpinBox(groupBox_2);
        count->setObjectName(QString::fromUtf8("count"));
        count->setAccelerated(true);
        count->setMaximum(100000);
        count->setValue(10);

        gridLayout_4->addWidget(count, 0, 1, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        groupBox_5 = new QGroupBox(dockWidgetContents);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_7 = new QGridLayout(groupBox_5);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        x = new QDoubleSpinBox(groupBox_5);
        x->setObjectName(QString::fromUtf8("x"));
        x->setWrapping(false);
        x->setAccelerated(true);
        x->setDecimals(2);
        x->setMinimum(0.01);
        x->setMaximum(10);
        x->setSingleStep(0.01);
        x->setValue(0.3);

        gridLayout_7->addWidget(x, 0, 1, 1, 1);

        label_10 = new QLabel(groupBox_5);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_7->addWidget(label_10, 0, 0, 1, 1);

        label_11 = new QLabel(groupBox_5);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_7->addWidget(label_11, 2, 0, 1, 1);

        y = new QDoubleSpinBox(groupBox_5);
        y->setObjectName(QString::fromUtf8("y"));
        y->setWrapping(false);
        y->setAccelerated(true);
        y->setDecimals(2);
        y->setMinimum(0.01);
        y->setMaximum(10);
        y->setSingleStep(0.01);
        y->setValue(0.3);

        gridLayout_7->addWidget(y, 2, 1, 1, 1);

        z = new QDoubleSpinBox(groupBox_5);
        z->setObjectName(QString::fromUtf8("z"));
        z->setWrapping(false);
        z->setAccelerated(true);
        z->setDecimals(2);
        z->setMinimum(0.01);
        z->setMaximum(10);
        z->setSingleStep(0.01);
        z->setValue(0.3);

        gridLayout_7->addWidget(z, 3, 1, 1, 1);

        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_7->addWidget(label_12, 3, 0, 1, 1);


        verticalLayout->addWidget(groupBox_5);

        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_5->addWidget(label_15, 2, 0, 1, 1);

        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_5->addWidget(label_16, 3, 0, 1, 1);

        label_19 = new QLabel(groupBox_3);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout_5->addWidget(label_19, 4, 0, 1, 1);

        inter_gravity = new QDoubleSpinBox(groupBox_3);
        inter_gravity->setObjectName(QString::fromUtf8("inter_gravity"));
        inter_gravity->setWrapping(false);
        inter_gravity->setAccelerated(true);
        inter_gravity->setDecimals(2);
        inter_gravity->setMinimum(0.01);
        inter_gravity->setMaximum(300);
        inter_gravity->setSingleStep(0.1);
        inter_gravity->setValue(1);

        gridLayout_5->addWidget(inter_gravity, 1, 1, 1, 1);

        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_5->addWidget(label_14, 1, 0, 1, 1);

        wall_resistance = new QCheckBox(groupBox_3);
        wall_resistance->setObjectName(QString::fromUtf8("wall_resistance"));
        wall_resistance->setChecked(true);

        gridLayout_5->addWidget(wall_resistance, 3, 1, 1, 1);

        label_20 = new QLabel(groupBox_3);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout_5->addWidget(label_20, 5, 0, 1, 1);

        earth_gravity = new QDoubleSpinBox(groupBox_3);
        earth_gravity->setObjectName(QString::fromUtf8("earth_gravity"));
        earth_gravity->setWrapping(false);
        earth_gravity->setAccelerated(true);
        earth_gravity->setDecimals(2);
        earth_gravity->setMinimum(0.01);
        earth_gravity->setMaximum(100);
        earth_gravity->setSingleStep(0.1);
        earth_gravity->setValue(9.81);

        gridLayout_5->addWidget(earth_gravity, 0, 1, 1, 1);

        label_13 = new QLabel(groupBox_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_5->addWidget(label_13, 0, 0, 1, 1);

        e_modul = new QDoubleSpinBox(groupBox_3);
        e_modul->setObjectName(QString::fromUtf8("e_modul"));
        e_modul->setWrapping(false);
        e_modul->setAccelerated(true);
        e_modul->setDecimals(2);
        e_modul->setMinimum(0.01);
        e_modul->setMaximum(1e+06);
        e_modul->setSingleStep(0.1);
        e_modul->setValue(50000);

        gridLayout_5->addWidget(e_modul, 4, 1, 1, 1);

        air_resistance = new QDoubleSpinBox(groupBox_3);
        air_resistance->setObjectName(QString::fromUtf8("air_resistance"));
        air_resistance->setWrapping(false);
        air_resistance->setAccelerated(true);
        air_resistance->setDecimals(2);
        air_resistance->setMinimum(0.01);
        air_resistance->setMaximum(100);
        air_resistance->setSingleStep(0.1);
        air_resistance->setValue(1);

        gridLayout_5->addWidget(air_resistance, 2, 1, 1, 1);

        poissons_ratio = new QDoubleSpinBox(groupBox_3);
        poissons_ratio->setObjectName(QString::fromUtf8("poissons_ratio"));
        poissons_ratio->setWrapping(false);
        poissons_ratio->setAccelerated(true);
        poissons_ratio->setDecimals(2);
        poissons_ratio->setMinimum(-1);
        poissons_ratio->setMaximum(0.5);
        poissons_ratio->setSingleStep(0.1);
        poissons_ratio->setValue(0.5);

        gridLayout_5->addWidget(poissons_ratio, 5, 1, 1, 1);

        label_21 = new QLabel(groupBox_3);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        gridLayout_5->addWidget(label_21, 6, 0, 1, 1);

        elasticity = new QDoubleSpinBox(groupBox_3);
        elasticity->setObjectName(QString::fromUtf8("elasticity"));
        elasticity->setWrapping(false);
        elasticity->setAccelerated(true);
        elasticity->setDecimals(2);
        elasticity->setMinimum(0);
        elasticity->setMaximum(10);
        elasticity->setSingleStep(0.1);
        elasticity->setValue(1);

        gridLayout_5->addWidget(elasticity, 6, 1, 1, 1);


        verticalLayout->addWidget(groupBox_3);

        Calculations->setWidget(dockWidgetContents);
#ifndef QT_NO_SHORTCUT
        label_6->setBuddy(count);
        label_18->setBuddy(radius_max);
        label_17->setBuddy(one_size);
        label_5->setBuddy(radius_min);
        label_10->setBuddy(x);
        label_11->setBuddy(y);
        label_12->setBuddy(z);
        label_15->setBuddy(air_resistance);
        label_16->setBuddy(wall_resistance);
        label_19->setBuddy(e_modul);
        label_14->setBuddy(inter_gravity);
        label_20->setBuddy(poissons_ratio);
        label_13->setBuddy(earth_gravity);
        label_21->setBuddy(elasticity);
#endif // QT_NO_SHORTCUT

        retranslateUi(Calculations);
        QObject::connect(one_size, SIGNAL(toggled(bool)), radius_max, SLOT(setDisabled(bool)));

        QMetaObject::connectSlotsByName(Calculations);
    } // setupUi

    void retranslateUi(QDockWidget *Calculations)
    {
        Calculations->setWindowTitle(QApplication::translate("Calculations", "Calculations", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Calculations", "Objects", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Calculations", "Object Count", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("Calculations", "Radius max.", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("Calculations", "One size", 0, QApplication::UnicodeUTF8));
        one_size->setText(QApplication::translate("Calculations", "yes", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Calculations", "Radius min.", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("Calculations", "Size", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Calculations", "X", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Calculations", "Y", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Calculations", "Z", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("Calculations", "Forces", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("Calculations", "Air resistance", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("Calculations", "Wall resistance", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("Calculations", "E modulus", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("Calculations", "InterGravity", 0, QApplication::UnicodeUTF8));
        wall_resistance->setText(QApplication::translate("Calculations", "yes", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("Calculations", "Poisson's ratio", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("Calculations", "EarthGravity", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("Calculations", "Elasticity", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Calculations: public Ui_Calculations {};
} // namespace Ui

QT_END_NAMESPACE

#endif // CALCULATIONSIK5186_H

/********************************************************************************
** Form generated from reading UI file 'RenderingBC5186.ui'
**
** Created: Tue Feb 12 19:27:58 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef RENDERINGBC5186_H
#define RENDERINGBC5186_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Rendering
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label_24;
    QLabel *label_22;
    QCheckBox *colours;
    QCheckBox *connect_trace;
    QLabel *label;
    QLabel *label_23;
    QDoubleSpinBox *fps;
    QCheckBox *traces;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_9;
    QDoubleSpinBox *z_rot;
    QDoubleSpinBox *y_rot;
    QDoubleSpinBox *x_rot;
    QLabel *label_27;
    QLabel *label_26;
    QLabel *label_25;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_6;
    QDoubleSpinBox *render_fps;
    QLabel *label_9;
    QPushButton *start;
    QLabel *label_3;
    QDoubleSpinBox *calc_fps;
    QPushButton *stop;
    QDoubleSpinBox *real_fps;
    QDoubleSpinBox *calc_speed;
    QProgressBar *frame_buffer;
    QLabel *label_2;
    QLabel *label_7;
    QLabel *label_4;
    QLabel *label_8;
    QDoubleSpinBox *real_speed;

    void setupUi(QDockWidget *Rendering)
    {
        if (Rendering->objectName().isEmpty())
            Rendering->setObjectName(QString::fromUtf8("Rendering"));
        Rendering->resize(232, 509);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_24 = new QLabel(groupBox);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        gridLayout_2->addWidget(label_24, 3, 0, 1, 1);

        label_22 = new QLabel(groupBox);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        gridLayout_2->addWidget(label_22, 1, 0, 1, 1);

        colours = new QCheckBox(groupBox);
        colours->setObjectName(QString::fromUtf8("colours"));
        colours->setChecked(true);

        gridLayout_2->addWidget(colours, 1, 1, 1, 1);

        connect_trace = new QCheckBox(groupBox);
        connect_trace->setObjectName(QString::fromUtf8("connect_trace"));
        connect_trace->setChecked(true);

        gridLayout_2->addWidget(connect_trace, 3, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_23 = new QLabel(groupBox);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        gridLayout_2->addWidget(label_23, 2, 0, 1, 1);

        fps = new QDoubleSpinBox(groupBox);
        fps->setObjectName(QString::fromUtf8("fps"));
        fps->setWrapping(false);
        fps->setAccelerated(true);
        fps->setDecimals(1);
        fps->setMinimum(0.1);
        fps->setMaximum(300);
        fps->setValue(60);

        gridLayout_2->addWidget(fps, 0, 1, 1, 1);

        traces = new QCheckBox(groupBox);
        traces->setObjectName(QString::fromUtf8("traces"));
        traces->setChecked(true);

        gridLayout_2->addWidget(traces, 2, 1, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_7 = new QGroupBox(dockWidgetContents);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        gridLayout_9 = new QGridLayout(groupBox_7);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        z_rot = new QDoubleSpinBox(groupBox_7);
        z_rot->setObjectName(QString::fromUtf8("z_rot"));
        z_rot->setWrapping(false);
        z_rot->setAccelerated(true);
        z_rot->setDecimals(2);
        z_rot->setMinimum(-10);
        z_rot->setMaximum(10);
        z_rot->setSingleStep(0.01);
        z_rot->setValue(0);

        gridLayout_9->addWidget(z_rot, 3, 1, 1, 1);

        y_rot = new QDoubleSpinBox(groupBox_7);
        y_rot->setObjectName(QString::fromUtf8("y_rot"));
        y_rot->setWrapping(false);
        y_rot->setAccelerated(true);
        y_rot->setDecimals(2);
        y_rot->setMinimum(-10);
        y_rot->setMaximum(10);
        y_rot->setSingleStep(0.01);
        y_rot->setValue(0.1);

        gridLayout_9->addWidget(y_rot, 2, 1, 1, 1);

        x_rot = new QDoubleSpinBox(groupBox_7);
        x_rot->setObjectName(QString::fromUtf8("x_rot"));
        x_rot->setWrapping(false);
        x_rot->setAccelerated(true);
        x_rot->setDecimals(2);
        x_rot->setMinimum(-10);
        x_rot->setMaximum(10);
        x_rot->setSingleStep(0.01);
        x_rot->setValue(0);

        gridLayout_9->addWidget(x_rot, 0, 1, 1, 1);

        label_27 = new QLabel(groupBox_7);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        gridLayout_9->addWidget(label_27, 3, 0, 1, 1);

        label_26 = new QLabel(groupBox_7);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        gridLayout_9->addWidget(label_26, 2, 0, 1, 1);

        label_25 = new QLabel(groupBox_7);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        gridLayout_9->addWidget(label_25, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox_7, 1, 0, 1, 1);

        groupBox_4 = new QGroupBox(dockWidgetContents);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout_6 = new QGridLayout(groupBox_4);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        render_fps = new QDoubleSpinBox(groupBox_4);
        render_fps->setObjectName(QString::fromUtf8("render_fps"));
        render_fps->setFrame(true);
        render_fps->setReadOnly(true);
        render_fps->setButtonSymbols(QAbstractSpinBox::NoButtons);
        render_fps->setMaximum(1e+06);
        render_fps->setSingleStep(0.01);

        gridLayout_6->addWidget(render_fps, 10, 1, 1, 1);

        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_6->addWidget(label_9, 7, 0, 1, 1);

        start = new QPushButton(groupBox_4);
        start->setObjectName(QString::fromUtf8("start"));

        gridLayout_6->addWidget(start, 1, 0, 1, 1);

        label_3 = new QLabel(groupBox_4);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_6->addWidget(label_3, 6, 0, 1, 1);

        calc_fps = new QDoubleSpinBox(groupBox_4);
        calc_fps->setObjectName(QString::fromUtf8("calc_fps"));
        calc_fps->setFrame(true);
        calc_fps->setReadOnly(true);
        calc_fps->setButtonSymbols(QAbstractSpinBox::NoButtons);
        calc_fps->setMaximum(1e+06);
        calc_fps->setSingleStep(0.01);

        gridLayout_6->addWidget(calc_fps, 6, 1, 1, 1);

        stop = new QPushButton(groupBox_4);
        stop->setObjectName(QString::fromUtf8("stop"));
        stop->setEnabled(false);

        gridLayout_6->addWidget(stop, 1, 1, 1, 1);

        real_fps = new QDoubleSpinBox(groupBox_4);
        real_fps->setObjectName(QString::fromUtf8("real_fps"));
        real_fps->setFrame(true);
        real_fps->setReadOnly(true);
        real_fps->setButtonSymbols(QAbstractSpinBox::NoButtons);
        real_fps->setMaximum(1e+06);
        real_fps->setSingleStep(0.01);

        gridLayout_6->addWidget(real_fps, 7, 1, 1, 1);

        calc_speed = new QDoubleSpinBox(groupBox_4);
        calc_speed->setObjectName(QString::fromUtf8("calc_speed"));
        calc_speed->setWrapping(false);
        calc_speed->setAccelerated(true);
        calc_speed->setDecimals(2);
        calc_speed->setMinimum(0.01);
        calc_speed->setMaximum(100);
        calc_speed->setSingleStep(0.1);
        calc_speed->setValue(0.1);

        gridLayout_6->addWidget(calc_speed, 3, 1, 1, 1);

        frame_buffer = new QProgressBar(groupBox_4);
        frame_buffer->setObjectName(QString::fromUtf8("frame_buffer"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_buffer->sizePolicy().hasHeightForWidth());
        frame_buffer->setSizePolicy(sizePolicy);
        frame_buffer->setValue(24);

        gridLayout_6->addWidget(frame_buffer, 11, 1, 1, 1);

        label_2 = new QLabel(groupBox_4);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_6->addWidget(label_2, 3, 0, 1, 1);

        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_6->addWidget(label_7, 11, 0, 1, 1);

        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_6->addWidget(label_4, 10, 0, 1, 1);

        label_8 = new QLabel(groupBox_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_6->addWidget(label_8, 4, 0, 1, 1);

        real_speed = new QDoubleSpinBox(groupBox_4);
        real_speed->setObjectName(QString::fromUtf8("real_speed"));
        real_speed->setFrame(true);
        real_speed->setReadOnly(true);
        real_speed->setButtonSymbols(QAbstractSpinBox::NoButtons);
        real_speed->setMaximum(1e+06);
        real_speed->setSingleStep(0.01);

        gridLayout_6->addWidget(real_speed, 4, 1, 1, 1);


        gridLayout->addWidget(groupBox_4, 2, 0, 1, 1);

        Rendering->setWidget(dockWidgetContents);
#ifndef QT_NO_SHORTCUT
        label_24->setBuddy(connect_trace);
        label_22->setBuddy(colours);
        label->setBuddy(fps);
        label_23->setBuddy(traces);
        label_27->setBuddy(z_rot);
        label_26->setBuddy(y_rot);
        label_25->setBuddy(x_rot);
        label_9->setBuddy(calc_fps);
        label_3->setBuddy(calc_fps);
        label_2->setBuddy(calc_speed);
        label_4->setBuddy(render_fps);
        label_8->setBuddy(calc_fps);
#endif // QT_NO_SHORTCUT

        retranslateUi(Rendering);
        QObject::connect(start, SIGNAL(clicked(bool)), stop, SLOT(setDisabled(bool)));
        QObject::connect(start, SIGNAL(clicked(bool)), start, SLOT(setEnabled(bool)));
        QObject::connect(stop, SIGNAL(clicked(bool)), start, SLOT(setDisabled(bool)));
        QObject::connect(stop, SIGNAL(clicked(bool)), stop, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(Rendering);
    } // setupUi

    void retranslateUi(QDockWidget *Rendering)
    {
        Rendering->setWindowTitle(QApplication::translate("Rendering", "Rendering", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Rendering", "Rendering", 0, QApplication::UnicodeUTF8));
        label_24->setText(QApplication::translate("Rendering", "Connect trace", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("Rendering", "Colours", 0, QApplication::UnicodeUTF8));
        colours->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
        connect_trace->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Rendering", "FPS", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("Rendering", "Traces", 0, QApplication::UnicodeUTF8));
        traces->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("Rendering", "Auto rotation", 0, QApplication::UnicodeUTF8));
        label_27->setText(QApplication::translate("Rendering", "Z axis", 0, QApplication::UnicodeUTF8));
        label_26->setText(QApplication::translate("Rendering", "Y axis", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("Rendering", "X axis", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("Rendering", "General", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Rendering", "Real calc. FPS", 0, QApplication::UnicodeUTF8));
        start->setText(QApplication::translate("Rendering", "Start", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Rendering", "Calc. FPS", 0, QApplication::UnicodeUTF8));
        stop->setText(QApplication::translate("Rendering", "Stop", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Rendering", "Speed", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Rendering", "Frame buffer", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Rendering", "Render FPS", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Rendering", "Real speed", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Rendering: public Ui_Rendering {};
} // namespace Ui

QT_END_NAMESPACE

#endif // RENDERINGBC5186_H
