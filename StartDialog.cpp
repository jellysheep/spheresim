
#include "StartDialog.h"
#include "ui_StartDialog_Qt5.h"
#include "EigenCalculator_Engine.h"
#include "OpenClCalculator.h"
#include "ThrustCalculator.h"
#include "FileCalculator.h"
#include <QTranslator>
#include <QLibraryInfo>

#include "Spheres.h"

StartDialog::StartDialog(){
	#if 0
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
		dlg = new Ui::StartDialog();
		dlg->setupUi(this);
		QObject::connect(this, SIGNAL(accepted()),
			this, SLOT(accepted_()), Qt::AutoConnection);
		QObject::connect(this, SIGNAL(rejected()),
			this, SLOT(rejected_()), Qt::AutoConnection);
		exec();
	#endif
}

void StartDialog::accepted_(){
	calc = NULL;
	///language:
	QTranslator qtTranslator;
	QTranslator sTranslator;
	QString localeName;
	if(dlg->german->isChecked()){
		localeName = "de_DE";
	}else{
		localeName = "en_GB";
	}
	qtTranslator.load("qt_" + localeName,
		QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	QApplication::installTranslator(&qtTranslator);
	QString localeStr = "translate_" + localeName;
	sTranslator.load(localeStr);
	printf("locale data: \"%s\"\n", localeStr.toLatin1().constData());
	QApplication::installTranslator(&sTranslator);
	
	///dimension:
	if(dlg->_3D->isChecked()){
		use3D = true;
	}else{
		use3D = false;
	}
	
	///magnitude:
	if(dlg->planets->isChecked()){
		magnitude = 1;
	}else{
		magnitude = 0;
	}
	curUnit = unitOfMagnitude[magnitude];
	
	///simulationType:
	if(dlg->read->isChecked()){
		calc = new FileCalculator();
	}else{
		if(use3D){
			calc = new EigenCalculator_Engine<3,true>();
		}else{
			calc = new EigenCalculator_Engine<2,false>();
		}
	}
	selected = true;
}

void StartDialog::rejected_(){
	calc = NULL;
	selected = true;
}

Calculator* StartDialog::getCalculator(){
	while(!selected);
	return calc;
}
