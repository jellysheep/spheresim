#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "Spheres.h"
#include <QDialog>
#include "Calculator.h"

namespace Ui {
	class Dialog;
}

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
