#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "ui_Dialog_Qt5.h"
#include "Spheres.h"
#include <QDialog>
#include "Calculator.h"

class Dialog : public QDialog {
	Q_OBJECT
protected:
	Ui::Dialog* dlg;
	bool selected;
	Calculator* calc;
public:
	Dialog();
	Calculator* getCalculator();
public slots:
	void accepted_();
	void rejected_();
};

#endif /* _DIALOG_H_ */
