
#include "EigenCalculator.h"
#include "Dialog.h"
#include "OpenClCalculator.h"
#include "FileCalculator.h"

Dialog::Dialog(){
	#if 0
		calc = new EigenCalculator();
		//calc = new FileCalculator();
		//calc = new OpenClCalculator();
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
	calc = NULL;
	#ifdef ENGINE_CPP
		if(dlg->cpp->isChecked()){
			calc = new EigenCalculator();
		}
	#endif
	#ifdef ENGINE_OPENCL
		if(dlg->opencl->isChecked()){
			calc = new OpenClCalculator();
		}
	#endif
	#ifdef ENGINE_READ
		if(dlg->read->isChecked()){
			calc = new FileCalculator();
		}
	#endif
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
