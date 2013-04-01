
#include "Dialog.h"
#include "ui_Dialog_Qt5.h"
#include "EigenCalculator_Engine.h"
#include "OpenClCalculator.h"
#include "ThrustCalculator.h"
#include "FileCalculator.h"

Dialog::Dialog(){
	#if 1
		#if 0
			if(use3D){
				calc = new ThrustCalculator3D();
			}else{
				calc = new ThrustCalculator2D();
			}
			selected = true;
		#else
			if(use3D){
				calc = new EigenCalculator_Engine<3,true>();
			}else{
				calc = new EigenCalculator_Engine<2,false>();
			}
			//calc = new FileCalculator();
			//calc = new OpenClCalculator();
			selected = true;
		#endif
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
		#ifdef ENGINE_THRUST
			dlg->thrust->setEnabled(true);
		#endif
		#ifdef ENGINE_READ
			dlg->read->setEnabled(true);
		#endif
		QObject::connect(this, SIGNAL(accepted()), 
			this, SLOT(accepted_()), Qt::AutoConnection);
		QObject::connect(this, SIGNAL(rejected()), 
			this, SLOT(rejected_()), Qt::AutoConnection);
		exec();
	#endif
}

void Dialog::accepted_(){
	calc = NULL;
	#ifdef ENGINE_CPP
		if(dlg->cpp->isChecked()){
			if(use3D){
				calc = new EigenCalculator_Engine<3,true>();
			}else{
				calc = new EigenCalculator_Engine<2,false>();
			}
		}
	#endif
	#ifdef ENGINE_OPENCL
		if(dlg->opencl->isChecked()){
			calc = new OpenClCalculator();
		}
	#endif
	#ifdef ENGINE_THRUST
		if(dlg->thrust->isChecked()){
			if(use3D){
				calc = new ThrustCalculator3D();
			}else{
				calc = new ThrustCalculator2D();
			}
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
