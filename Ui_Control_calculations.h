/********************************************************************************
** Form generated from reading UI file 'Calculations_Qt5M29424.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef CALCULATIONS_QT5M29424_H
#define CALCULATIONS_QT5M29424_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Calculations
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *label_7;
    QSpinBox *visible_count;
    QLabel *label_5;
    QDoubleSpinBox *radius_max;
    QDoubleSpinBox *radius_min;
    QLabel *label_6;
    QSpinBox *count;
    QLabel *label_18;
    QCheckBox *render;
    QCheckBox *save;
    QCheckBox *one_size;
    QCheckBox *wireframe;
    QPushButton *saveConfig;
    QPushButton *loadConfig;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_7;
    QDoubleSpinBox *z;
    QDoubleSpinBox *x;
    QLabel *label_11;
    QLabel *label_10;
    QLabel *label_12;
    QDoubleSpinBox *y;
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
            Calculations->setObjectName(QStringLiteral("Calculations"));
        Calculations->resize(262, 566);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 133));
        groupBox_2->setMaximumSize(QSize(16777215, 16777215));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 1, 0, 1, 1);

        visible_count = new QSpinBox(groupBox_2);
        visible_count->setObjectName(QStringLiteral("visible_count"));
        visible_count->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        visible_count->setAccelerated(true);
        visible_count->setMaximum(100000);
        visible_count->setValue(10);

        gridLayout->addWidget(visible_count, 1, 1, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 3, 0, 1, 1);

        radius_max = new QDoubleSpinBox(groupBox_2);
        radius_max->setObjectName(QStringLiteral("radius_max"));
        radius_max->setEnabled(false);
        radius_max->setWrapping(false);
        radius_max->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        radius_max->setAccelerated(true);
        radius_max->setDecimals(3);
        radius_max->setMinimum(0.001);
        radius_max->setMaximum(0.1);
        radius_max->setSingleStep(0.001);
        radius_max->setValue(0.01);

        gridLayout->addWidget(radius_max, 5, 1, 1, 1);

        radius_min = new QDoubleSpinBox(groupBox_2);
        radius_min->setObjectName(QStringLiteral("radius_min"));
        radius_min->setWrapping(false);
        radius_min->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        radius_min->setAccelerated(true);
        radius_min->setDecimals(3);
        radius_min->setMinimum(0.001);
        radius_min->setMaximum(0.1);
        radius_min->setSingleStep(0.001);
        radius_min->setValue(0.01);

        gridLayout->addWidget(radius_min, 3, 1, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 0, 0, 1, 1);

        count = new QSpinBox(groupBox_2);
        count->setObjectName(QStringLiteral("count"));
        count->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        count->setAccelerated(true);
        count->setMaximum(100000);
        count->setValue(10);

        gridLayout->addWidget(count, 0, 1, 1, 1);

        label_18 = new QLabel(groupBox_2);
        label_18->setObjectName(QStringLiteral("label_18"));

        gridLayout->addWidget(label_18, 5, 0, 1, 1);

        render = new QCheckBox(groupBox_2);
        render->setObjectName(QStringLiteral("render"));

        gridLayout->addWidget(render, 7, 0, 1, 1);

        save = new QCheckBox(groupBox_2);
        save->setObjectName(QStringLiteral("save"));

        gridLayout->addWidget(save, 7, 1, 1, 1);

        one_size = new QCheckBox(groupBox_2);
        one_size->setObjectName(QStringLiteral("one_size"));
        one_size->setChecked(true);

        gridLayout->addWidget(one_size, 2, 0, 1, 1);

        wireframe = new QCheckBox(groupBox_2);
        wireframe->setObjectName(QStringLiteral("wireframe"));
        wireframe->setChecked(false);

        gridLayout->addWidget(wireframe, 2, 1, 1, 1);

        saveConfig = new QPushButton(groupBox_2);
        saveConfig->setObjectName(QStringLiteral("saveConfig"));

        gridLayout->addWidget(saveConfig, 8, 0, 1, 1);

        loadConfig = new QPushButton(groupBox_2);
        loadConfig->setObjectName(QStringLiteral("loadConfig"));

        gridLayout->addWidget(loadConfig, 8, 1, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        groupBox_5 = new QGroupBox(dockWidgetContents);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_7 = new QGridLayout(groupBox_5);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        z = new QDoubleSpinBox(groupBox_5);
        z->setObjectName(QStringLiteral("z"));
        z->setWrapping(false);
        z->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        z->setAccelerated(true);
        z->setDecimals(2);
        z->setMinimum(0.01);
        z->setMaximum(10);
        z->setSingleStep(0.01);
        z->setValue(0.3);

        gridLayout_7->addWidget(z, 3, 1, 1, 1);

        x = new QDoubleSpinBox(groupBox_5);
        x->setObjectName(QStringLiteral("x"));
        x->setWrapping(false);
        x->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        x->setAccelerated(true);
        x->setDecimals(2);
        x->setMinimum(0.01);
        x->setMaximum(10);
        x->setSingleStep(0.01);
        x->setValue(0.3);

        gridLayout_7->addWidget(x, 0, 1, 1, 1);

        label_11 = new QLabel(groupBox_5);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout_7->addWidget(label_11, 2, 0, 1, 1);

        label_10 = new QLabel(groupBox_5);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_7->addWidget(label_10, 0, 0, 1, 1);

        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QStringLiteral("label_12"));

        gridLayout_7->addWidget(label_12, 3, 0, 1, 1);

        y = new QDoubleSpinBox(groupBox_5);
        y->setObjectName(QStringLiteral("y"));
        y->setWrapping(false);
        y->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        y->setAccelerated(true);
        y->setDecimals(2);
        y->setMinimum(0.01);
        y->setMaximum(10);
        y->setSingleStep(0.01);
        y->setValue(0.3);

        gridLayout_7->addWidget(y, 2, 1, 1, 1);


        verticalLayout->addWidget(groupBox_5);

        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName(QStringLiteral("label_15"));

        gridLayout_5->addWidget(label_15, 2, 0, 1, 1);

        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName(QStringLiteral("label_16"));

        gridLayout_5->addWidget(label_16, 3, 0, 1, 1);

        label_19 = new QLabel(groupBox_3);
        label_19->setObjectName(QStringLiteral("label_19"));

        gridLayout_5->addWidget(label_19, 4, 0, 1, 1);

        inter_gravity = new QDoubleSpinBox(groupBox_3);
        inter_gravity->setObjectName(QStringLiteral("inter_gravity"));
        inter_gravity->setWrapping(false);
        inter_gravity->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        inter_gravity->setAccelerated(true);
        inter_gravity->setDecimals(2);
        inter_gravity->setMinimum(-300);
        inter_gravity->setMaximum(300);
        inter_gravity->setSingleStep(0.1);
        inter_gravity->setValue(1);

        gridLayout_5->addWidget(inter_gravity, 1, 1, 1, 1);

        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_5->addWidget(label_14, 1, 0, 1, 1);

        wall_resistance = new QCheckBox(groupBox_3);
        wall_resistance->setObjectName(QStringLiteral("wall_resistance"));
        wall_resistance->setChecked(true);

        gridLayout_5->addWidget(wall_resistance, 3, 1, 1, 1);

        label_20 = new QLabel(groupBox_3);
        label_20->setObjectName(QStringLiteral("label_20"));

        gridLayout_5->addWidget(label_20, 5, 0, 1, 1);

        earth_gravity = new QDoubleSpinBox(groupBox_3);
        earth_gravity->setObjectName(QStringLiteral("earth_gravity"));
        earth_gravity->setWrapping(false);
        earth_gravity->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        earth_gravity->setAccelerated(true);
        earth_gravity->setDecimals(2);
        earth_gravity->setMinimum(-100);
        earth_gravity->setMaximum(100);
        earth_gravity->setSingleStep(0.1);
        earth_gravity->setValue(9.81);

        gridLayout_5->addWidget(earth_gravity, 0, 1, 1, 1);

        label_13 = new QLabel(groupBox_3);
        label_13->setObjectName(QStringLiteral("label_13"));

        gridLayout_5->addWidget(label_13, 0, 0, 1, 1);

        e_modul = new QDoubleSpinBox(groupBox_3);
        e_modul->setObjectName(QStringLiteral("e_modul"));
        e_modul->setWrapping(false);
        e_modul->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        e_modul->setAccelerated(true);
        e_modul->setDecimals(2);
        e_modul->setMinimum(0.01);
        e_modul->setMaximum(1000);
        e_modul->setSingleStep(0.02);
        e_modul->setValue(0.1);

        gridLayout_5->addWidget(e_modul, 4, 1, 1, 1);

        air_resistance = new QDoubleSpinBox(groupBox_3);
        air_resistance->setObjectName(QStringLiteral("air_resistance"));
        air_resistance->setWrapping(false);
        air_resistance->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        air_resistance->setAccelerated(true);
        air_resistance->setDecimals(2);
        air_resistance->setMinimum(0);
        air_resistance->setMaximum(100);
        air_resistance->setSingleStep(0.1);
        air_resistance->setValue(1);

        gridLayout_5->addWidget(air_resistance, 2, 1, 1, 1);

        poissons_ratio = new QDoubleSpinBox(groupBox_3);
        poissons_ratio->setObjectName(QStringLiteral("poissons_ratio"));
        poissons_ratio->setWrapping(false);
        poissons_ratio->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        poissons_ratio->setAccelerated(true);
        poissons_ratio->setDecimals(2);
        poissons_ratio->setMinimum(-1);
        poissons_ratio->setMaximum(1);
        poissons_ratio->setSingleStep(0.1);
        poissons_ratio->setValue(0.5);

        gridLayout_5->addWidget(poissons_ratio, 5, 1, 1, 1);

        label_21 = new QLabel(groupBox_3);
        label_21->setObjectName(QStringLiteral("label_21"));

        gridLayout_5->addWidget(label_21, 6, 0, 1, 1);

        elasticity = new QDoubleSpinBox(groupBox_3);
        elasticity->setObjectName(QStringLiteral("elasticity"));
        elasticity->setWrapping(false);
        elasticity->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
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
        label_7->setBuddy(count);
        label_5->setBuddy(radius_min);
        label_6->setBuddy(count);
        label_18->setBuddy(radius_max);
        label_11->setBuddy(y);
        label_10->setBuddy(x);
        label_12->setBuddy(z);
        label_15->setBuddy(air_resistance);
        label_16->setBuddy(wall_resistance);
        label_19->setBuddy(e_modul);
        label_14->setBuddy(inter_gravity);
        label_20->setBuddy(poissons_ratio);
        label_13->setBuddy(earth_gravity);
        label_21->setBuddy(elasticity);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(count, visible_count);
        QWidget::setTabOrder(visible_count, one_size);
        QWidget::setTabOrder(one_size, wireframe);
        QWidget::setTabOrder(wireframe, radius_min);
        QWidget::setTabOrder(radius_min, radius_max);
        QWidget::setTabOrder(radius_max, render);
        QWidget::setTabOrder(render, save);
        QWidget::setTabOrder(save, saveConfig);
        QWidget::setTabOrder(saveConfig, loadConfig);
        QWidget::setTabOrder(loadConfig, x);
        QWidget::setTabOrder(x, y);
        QWidget::setTabOrder(y, z);
        QWidget::setTabOrder(z, earth_gravity);
        QWidget::setTabOrder(earth_gravity, inter_gravity);
        QWidget::setTabOrder(inter_gravity, air_resistance);
        QWidget::setTabOrder(air_resistance, wall_resistance);
        QWidget::setTabOrder(wall_resistance, e_modul);
        QWidget::setTabOrder(e_modul, poissons_ratio);
        QWidget::setTabOrder(poissons_ratio, elasticity);

        retranslateUi(Calculations);
        QObject::connect(one_size, SIGNAL(toggled(bool)), radius_max, SLOT(setDisabled(bool)));
        QObject::connect(save, SIGNAL(toggled(bool)), visible_count, SLOT(setDisabled(bool)));
        QObject::connect(save, SIGNAL(toggled(bool)), count, SLOT(setDisabled(bool)));

        QMetaObject::connectSlotsByName(Calculations);
    } // setupUi

    void retranslateUi(QDockWidget *Calculations)
    {
        Calculations->setWindowTitle(QApplication::translate("Calculations", "Calculations", 0));
        groupBox_2->setTitle(QApplication::translate("Calculations", "Objects", 0));
        label_7->setText(QApplication::translate("Calculations", "Visible objects", 0));
        label_5->setText(QApplication::translate("Calculations", "Radius min.", 0));
        radius_max->setSuffix(QApplication::translate("Calculations", " m", 0));
        radius_min->setSuffix(QApplication::translate("Calculations", " m", 0));
        label_6->setText(QApplication::translate("Calculations", "Object count", 0));
        label_18->setText(QApplication::translate("Calculations", "Radius max.", 0));
        render->setText(QApplication::translate("Calculations", "Render", 0));
        save->setText(QApplication::translate("Calculations", "Save", 0));
        one_size->setText(QApplication::translate("Calculations", "One size", 0));
        wireframe->setText(QApplication::translate("Calculations", "Wireframe", 0));
        saveConfig->setText(QApplication::translate("Calculations", "Save config...", 0));
        loadConfig->setText(QApplication::translate("Calculations", "Load config...", 0));
        groupBox_5->setTitle(QApplication::translate("Calculations", "Size", 0));
        z->setSuffix(QApplication::translate("Calculations", " m", 0));
        x->setSuffix(QApplication::translate("Calculations", " m", 0));
        label_11->setText(QApplication::translate("Calculations", "Y", 0));
        label_10->setText(QApplication::translate("Calculations", "X", 0));
        label_12->setText(QApplication::translate("Calculations", "Z", 0));
        y->setSuffix(QApplication::translate("Calculations", " m", 0));
        groupBox_3->setTitle(QApplication::translate("Calculations", "Forces", 0));
        label_15->setText(QApplication::translate("Calculations", "Air resistance", 0));
        label_16->setText(QApplication::translate("Calculations", "Wall resistance", 0));
        label_19->setText(QApplication::translate("Calculations", "E modulus", 0));
        inter_gravity->setSuffix(QApplication::translate("Calculations", " x", 0));
        label_14->setText(QApplication::translate("Calculations", "InterGravity", 0));
        wall_resistance->setText(QApplication::translate("Calculations", "yes", 0));
        label_20->setText(QApplication::translate("Calculations", "Poisson's ratio", 0));
        earth_gravity->setSuffix(QApplication::translate("Calculations", " m/s\302\262", 0));
        label_13->setText(QApplication::translate("Calculations", "EarthGravity", 0));
        e_modul->setSuffix(QApplication::translate("Calculations", " GPa", 0));
        air_resistance->setSuffix(QApplication::translate("Calculations", " x", 0));
        label_21->setText(QApplication::translate("Calculations", "Elasticity", 0));
    } // retranslateUi

};

namespace Ui {
    class Calculations: public Ui_Calculations {};
} // namespace Ui

QT_END_NAMESPACE

#endif // CALCULATIONS_QT5M29424_H
