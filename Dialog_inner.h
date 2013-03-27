/********************************************************************************
** Form generated from reading UI file 'Dialog_Qt5T29424.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DIALOG_QT5T29424_H
#define DIALOG_QT5T29424_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QRadioButton *opencl;
    QRadioButton *cpp;
    QRadioButton *read;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->setWindowModality(Qt::WindowModal);
        Dialog->resize(217, 157);
        Dialog->setSizeGripEnabled(false);
        Dialog->setModal(true);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        opencl = new QRadioButton(Dialog);
        opencl->setObjectName(QStringLiteral("opencl"));
        opencl->setEnabled(false);

        verticalLayout->addWidget(opencl);

        cpp = new QRadioButton(Dialog);
        cpp->setObjectName(QStringLiteral("cpp"));
        cpp->setEnabled(false);

        verticalLayout->addWidget(cpp);

        read = new QRadioButton(Dialog);
        read->setObjectName(QStringLiteral("read"));
        read->setEnabled(false);

        verticalLayout->addWidget(read);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "SphereSim", 0));
        opencl->setText(QApplication::translate("Dialog", "OpenCL kernel", 0));
        cpp->setText(QApplication::translate("Dialog", "C++ with Eigen library", 0));
        read->setText(QApplication::translate("Dialog", "Read spheres from file", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DIALOG_QT5T29424_H
