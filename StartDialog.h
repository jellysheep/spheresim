#ifndef _START_DIALOG_H_
#define _START_DIALOG_H_
#include <QDialog>
#include "Calculator.h"

namespace Ui {
	class StartDialog;
}

class StartDialog : public QDialog {
	Q_OBJECT
protected:
	Ui::StartDialog* dlg;
	bool selected;
	Calculator* calc;
public:
	StartDialog();
	Calculator* getCalculator();
public slots:
	void accepted_();
	void rejected_();
};

#endif /* _START_DIALOG_H_ */
