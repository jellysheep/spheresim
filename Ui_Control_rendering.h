/********************************************************************************
** Form generated from reading UI file 'RenderingJvc965.ui'
**
** Created: Fri Feb 22 16:08:06 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef RENDERINGJVC965_H
#define RENDERINGJVC965_H

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
    QLabel *label;
    QDoubleSpinBox *fps;
    QDoubleSpinBox *trace_length;
    QLabel *label_30;
    QCheckBox *colours;
    QCheckBox *traces;
    QCheckBox *connect_trace;
    QCheckBox *lights;
    QCheckBox *reflections;
    QCheckBox *cube;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_9;
    QDoubleSpinBox *z_rot;
    QDoubleSpinBox *y_rot;
    QDoubleSpinBox *x_rot;
    QLabel *label_27;
    QLabel *label_26;
    QLabel *label_25;
    QPushButton *reset_view;
    QLabel *label_5;
    QLabel *label_6;
    QDoubleSpinBox *hue_rot;
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
        Rendering->resize(243, 594);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Rendering->sizePolicy().hasHeightForWidth());
        Rendering->setSizePolicy(sizePolicy);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        fps = new QDoubleSpinBox(groupBox);
        fps->setObjectName(QString::fromUtf8("fps"));
        fps->setWrapping(false);
        fps->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        fps->setAccelerated(true);
        fps->setDecimals(1);
        fps->setMinimum(0.1);
        fps->setMaximum(300);
        fps->setValue(60);

        gridLayout_2->addWidget(fps, 0, 1, 1, 1);

        trace_length = new QDoubleSpinBox(groupBox);
        trace_length->setObjectName(QString::fromUtf8("trace_length"));
        trace_length->setWrapping(false);
        trace_length->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        trace_length->setAccelerated(true);
        trace_length->setDecimals(1);
        trace_length->setMinimum(0);
        trace_length->setMaximum(100);
        trace_length->setSingleStep(0.1);
        trace_length->setValue(100);

        gridLayout_2->addWidget(trace_length, 4, 1, 1, 1);

        label_30 = new QLabel(groupBox);
        label_30->setObjectName(QString::fromUtf8("label_30"));

        gridLayout_2->addWidget(label_30, 4, 0, 1, 1);

        colours = new QCheckBox(groupBox);
        colours->setObjectName(QString::fromUtf8("colours"));
        colours->setChecked(true);

        gridLayout_2->addWidget(colours, 2, 0, 1, 1);

        traces = new QCheckBox(groupBox);
        traces->setObjectName(QString::fromUtf8("traces"));
        traces->setChecked(true);

        gridLayout_2->addWidget(traces, 3, 0, 1, 1);

        connect_trace = new QCheckBox(groupBox);
        connect_trace->setObjectName(QString::fromUtf8("connect_trace"));
        connect_trace->setChecked(true);

        gridLayout_2->addWidget(connect_trace, 3, 1, 1, 1);

        lights = new QCheckBox(groupBox);
        lights->setObjectName(QString::fromUtf8("lights"));
        lights->setChecked(false);

        gridLayout_2->addWidget(lights, 2, 1, 1, 1);

        reflections = new QCheckBox(groupBox);
        reflections->setObjectName(QString::fromUtf8("reflections"));
        reflections->setChecked(false);

        gridLayout_2->addWidget(reflections, 5, 1, 1, 1);

        cube = new QCheckBox(groupBox);
        cube->setObjectName(QString::fromUtf8("cube"));
        cube->setChecked(false);

        gridLayout_2->addWidget(cube, 5, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_7 = new QGroupBox(dockWidgetContents);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        gridLayout_9 = new QGridLayout(groupBox_7);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        z_rot = new QDoubleSpinBox(groupBox_7);
        z_rot->setObjectName(QString::fromUtf8("z_rot"));
        z_rot->setWrapping(false);
        z_rot->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
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
        y_rot->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
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
        x_rot->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
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

        reset_view = new QPushButton(groupBox_7);
        reset_view->setObjectName(QString::fromUtf8("reset_view"));

        gridLayout_9->addWidget(reset_view, 4, 1, 1, 1);

        label_5 = new QLabel(groupBox_7);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_9->addWidget(label_5, 4, 0, 1, 1);

        label_6 = new QLabel(groupBox_7);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_9->addWidget(label_6, 5, 0, 1, 1);

        hue_rot = new QDoubleSpinBox(groupBox_7);
        hue_rot->setObjectName(QString::fromUtf8("hue_rot"));
        hue_rot->setWrapping(false);
        hue_rot->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        hue_rot->setAccelerated(true);
        hue_rot->setDecimals(2);
        hue_rot->setMinimum(-10);
        hue_rot->setMaximum(10);
        hue_rot->setSingleStep(0.01);
        hue_rot->setValue(0);

        gridLayout_9->addWidget(hue_rot, 5, 1, 1, 1);


        gridLayout->addWidget(groupBox_7, 2, 0, 1, 1);

        groupBox_4 = new QGroupBox(dockWidgetContents);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout_6 = new QGridLayout(groupBox_4);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        render_fps = new QDoubleSpinBox(groupBox_4);
        render_fps->setObjectName(QString::fromUtf8("render_fps"));
        render_fps->setFrame(true);
        render_fps->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        render_fps->setReadOnly(true);
        render_fps->setButtonSymbols(QAbstractSpinBox::NoButtons);
        render_fps->setDecimals(1);
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
        calc_fps->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        calc_fps->setReadOnly(true);
        calc_fps->setButtonSymbols(QAbstractSpinBox::NoButtons);
        calc_fps->setDecimals(1);
        calc_fps->setMaximum(1e+07);
        calc_fps->setSingleStep(0.01);

        gridLayout_6->addWidget(calc_fps, 6, 1, 1, 1);

        stop = new QPushButton(groupBox_4);
        stop->setObjectName(QString::fromUtf8("stop"));
        stop->setEnabled(false);

        gridLayout_6->addWidget(stop, 1, 1, 1, 1);

        real_fps = new QDoubleSpinBox(groupBox_4);
        real_fps->setObjectName(QString::fromUtf8("real_fps"));
        real_fps->setFrame(true);
        real_fps->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        real_fps->setReadOnly(true);
        real_fps->setButtonSymbols(QAbstractSpinBox::NoButtons);
        real_fps->setDecimals(1);
        real_fps->setMaximum(9e+07);
        real_fps->setSingleStep(0.01);

        gridLayout_6->addWidget(real_fps, 7, 1, 1, 1);

        calc_speed = new QDoubleSpinBox(groupBox_4);
        calc_speed->setObjectName(QString::fromUtf8("calc_speed"));
        calc_speed->setWrapping(false);
        calc_speed->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        calc_speed->setAccelerated(true);
        calc_speed->setDecimals(3);
        calc_speed->setMinimum(0.01);
        calc_speed->setMaximum(1000);
        calc_speed->setSingleStep(0.02);
        calc_speed->setValue(0.1);

        gridLayout_6->addWidget(calc_speed, 3, 1, 1, 1);

        frame_buffer = new QProgressBar(groupBox_4);
        frame_buffer->setObjectName(QString::fromUtf8("frame_buffer"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_buffer->sizePolicy().hasHeightForWidth());
        frame_buffer->setSizePolicy(sizePolicy1);
        frame_buffer->setValue(24);
        frame_buffer->setInvertedAppearance(false);

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
        real_speed->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        real_speed->setReadOnly(true);
        real_speed->setButtonSymbols(QAbstractSpinBox::NoButtons);
        real_speed->setDecimals(3);
        real_speed->setMaximum(100000);
        real_speed->setSingleStep(0.01);

        gridLayout_6->addWidget(real_speed, 4, 1, 1, 1);


        gridLayout->addWidget(groupBox_4, 4, 0, 1, 1);

        Rendering->setWidget(dockWidgetContents);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(fps);
        label_30->setBuddy(traces);
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
        label->setText(QApplication::translate("Rendering", "FPS", 0, QApplication::UnicodeUTF8));
        fps->setSuffix(QApplication::translate("Rendering", " fps", 0, QApplication::UnicodeUTF8));
        trace_length->setSuffix(QApplication::translate("Rendering", " %", 0, QApplication::UnicodeUTF8));
        label_30->setText(QApplication::translate("Rendering", "Trace length", 0, QApplication::UnicodeUTF8));
        colours->setText(QApplication::translate("Rendering", "Colors", 0, QApplication::UnicodeUTF8));
        traces->setText(QApplication::translate("Rendering", "Traces", 0, QApplication::UnicodeUTF8));
        connect_trace->setText(QApplication::translate("Rendering", "Connect tr.", 0, QApplication::UnicodeUTF8));
        lights->setText(QApplication::translate("Rendering", "Show lights", 0, QApplication::UnicodeUTF8));
        reflections->setText(QApplication::translate("Rendering", "Reflections", 0, QApplication::UnicodeUTF8));
        cube->setText(QApplication::translate("Rendering", "Show cube", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("Rendering", "Auto rotation", 0, QApplication::UnicodeUTF8));
        z_rot->setSuffix(QApplication::translate("Rendering", "\302\260", 0, QApplication::UnicodeUTF8));
        y_rot->setSuffix(QApplication::translate("Rendering", "\302\260", 0, QApplication::UnicodeUTF8));
        x_rot->setSuffix(QApplication::translate("Rendering", "\302\260", 0, QApplication::UnicodeUTF8));
        label_27->setText(QApplication::translate("Rendering", "Z axis", 0, QApplication::UnicodeUTF8));
        label_26->setText(QApplication::translate("Rendering", "Y axis", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("Rendering", "X axis", 0, QApplication::UnicodeUTF8));
        reset_view->setText(QApplication::translate("Rendering", "Reset", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Rendering", "Reset view", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Rendering", "Hue rotation", 0, QApplication::UnicodeUTF8));
        hue_rot->setSuffix(QApplication::translate("Rendering", "\302\260", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("Rendering", "General", 0, QApplication::UnicodeUTF8));
        render_fps->setSuffix(QApplication::translate("Rendering", " fps", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Rendering", "Real calc. FPS", 0, QApplication::UnicodeUTF8));
        start->setText(QApplication::translate("Rendering", "Start", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Rendering", "Calc. FPS", 0, QApplication::UnicodeUTF8));
        calc_fps->setSuffix(QApplication::translate("Rendering", " fps", 0, QApplication::UnicodeUTF8));
        stop->setText(QApplication::translate("Rendering", "Stop", 0, QApplication::UnicodeUTF8));
        real_fps->setSuffix(QApplication::translate("Rendering", " fps", 0, QApplication::UnicodeUTF8));
        calc_speed->setSuffix(QApplication::translate("Rendering", " x", 0, QApplication::UnicodeUTF8));
        frame_buffer->setFormat(QApplication::translate("Rendering", "%p %", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Rendering", "Speed", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Rendering", "Frame buffer", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Rendering", "Render FPS", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Rendering", "Real speed", 0, QApplication::UnicodeUTF8));
        real_speed->setSuffix(QApplication::translate("Rendering", " x", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Rendering: public Ui_Rendering {};
} // namespace Ui

QT_END_NAMESPACE

#endif // RENDERINGJVC965_H
