#ifndef _START_DIALOG_H_
#define _START_DIALOG_H_
#include <QDialog>
#include "Calculator.h"
#include <QTranslator>

namespace Ui {
	class StartDialog;
}

class StartDialog : public QDialog {
	Q_OBJECT
protected:
	Ui::StartDialog* dlg;
	bool selected;
	Calculator* calc;
	QApplication* app;
	QTranslator* qtTranslator;
	QTranslator* sTranslator;
public:
	StartDialog(QApplication* a);
	Calculator* getCalculator();
public slots:
	void accepted_();
	void rejected_();
};

#endif /* _START_DIALOG_H_ */
