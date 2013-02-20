
#include "EigenCalculator.h"
#include "Dialog.h"
#include "OpenClCalculator.h"

Dialog::Dialog(){
	#if 1
		calc = new EigenCalculator();
		selected = true;
	#else
		selected = false;
		dlg = new Ui::Dialog();
		dlg->setupUi(this);
		#ifdef ENGINE_CPP
			dlg->cpp->setEnabled(true);
		#endif
		#ifdef ENGINE_OPENCL
			dlg->opencl->setEnabled(true);
		#endif
		#ifdef ENGINE_READ
			dlg->read->setEnabled(true);
		#endif
		QObject::connect(this, SIGNAL(accepted()), 
			this, SLOT(accepted_()), Qt::DirectConnection);
		QObject::connect(this, SIGNAL(rejected()), 
			this, SLOT(rejected_()), Qt::DirectConnection);
		exec();
	#endif
}

void Dialog::accepted_(){
	if(dlg->cpp->isChecked()){
		calc = new EigenCalculator();
	}else if(dlg->opencl->isChecked()){
		calc = new OpenClCalculator();
	}else if(dlg->read->isChecked()){
		calc = new EigenCalculator();
	}else{
		calc = NULL;
	}
	selected = true;
}

void Dialog::rejected_(){
	calc = NULL;
	selected = true;
}

Calculator* Dialog::getCalculator(){
	while(!selected);
	return calc;
}
