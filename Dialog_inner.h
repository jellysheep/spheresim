/********************************************************************************
** Form generated from reading UI file 'Dialogab1521.ui'
**
** Created: Thu Feb 28 11:33:50 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DIALOGAB1521_H
#define DIALOGAB1521_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

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
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->setWindowModality(Qt::WindowModal);
        Dialog->resize(217, 157);
        Dialog->setSizeGripEnabled(false);
        Dialog->setModal(true);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        opencl = new QRadioButton(Dialog);
        opencl->setObjectName(QString::fromUtf8("opencl"));
        opencl->setEnabled(false);

        verticalLayout->addWidget(opencl);

        cpp = new QRadioButton(Dialog);
        cpp->setObjectName(QString::fromUtf8("cpp"));
        cpp->setEnabled(false);

        verticalLayout->addWidget(cpp);

        read = new QRadioButton(Dialog);
        read->setObjectName(QString::fromUtf8("read"));
        read->setEnabled(false);

        verticalLayout->addWidget(read);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
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
        Dialog->setWindowTitle(QApplication::translate("Dialog", "SphereSim", 0, QApplication::UnicodeUTF8));
        opencl->setText(QApplication::translate("Dialog", "OpenCL kernel", 0, QApplication::UnicodeUTF8));
        cpp->setText(QApplication::translate("Dialog", "C++ with Eigen library", 0, QApplication::UnicodeUTF8));
        read->setText(QApplication::translate("Dialog", "Read spheres from file", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DIALOGAB1521_H
