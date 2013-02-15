/********************************************************************************
** Form generated from reading UI file 'Renderingy15477.ui'
**
** Created: Thu Feb 14 12:34:16 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef RENDERINGY15477_H
#define RENDERINGY15477_H

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
    QLabel *label_28;
    QCheckBox *wireframe;
    QLabel *label_22;
    QCheckBox *colours;
    QCheckBox *connect_trace;
    QLabel *label;
    QLabel *label_23;
    QDoubleSpinBox *fps;
    QCheckBox *traces;
    QLabel *label_29;
    QCheckBox *reflections;
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
        Rendering->resize(232, 555);
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

        label_28 = new QLabel(groupBox);
        label_28->setObjectName(QString::fromUtf8("label_28"));

        gridLayout_2->addWidget(label_28, 4, 0, 1, 1);

        wireframe = new QCheckBox(groupBox);
        wireframe->setObjectName(QString::fromUtf8("wireframe"));
        wireframe->setChecked(false);

        gridLayout_2->addWidget(wireframe, 4, 1, 1, 1);

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

        label_29 = new QLabel(groupBox);
        label_29->setObjectName(QString::fromUtf8("label_29"));

        gridLayout_2->addWidget(label_29, 5, 0, 1, 1);

        reflections = new QCheckBox(groupBox);
        reflections->setObjectName(QString::fromUtf8("reflections"));
        reflections->setChecked(false);

        gridLayout_2->addWidget(reflections, 5, 1, 1, 1);


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
        label_28->setBuddy(connect_trace);
        label_22->setBuddy(colours);
        label->setBuddy(fps);
        label_23->setBuddy(traces);
        label_29->setBuddy(connect_trace);
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
        label_28->setText(QApplication::translate("Rendering", "Wireframe", 0, QApplication::UnicodeUTF8));
        wireframe->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("Rendering", "Colours", 0, QApplication::UnicodeUTF8));
        colours->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
        connect_trace->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Rendering", "FPS", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("Rendering", "Traces", 0, QApplication::UnicodeUTF8));
        traces->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
        label_29->setText(QApplication::translate("Rendering", "Reflections", 0, QApplication::UnicodeUTF8));
        reflections->setText(QApplication::translate("Rendering", "yes", 0, QApplication::UnicodeUTF8));
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

#endif // RENDERINGY15477_H
