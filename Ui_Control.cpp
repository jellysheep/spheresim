#include "Ui_Control.h"
#include "Circles.h"
#include "Calculator.h"
#include <QtGui>
#include <Qt/qdockwidget.h>

#define cc (const QObject*)
//const-cast
 

Control::Control(GLWidget* g, Calculator* c, StatusViewer* s):QMainWindow(),glw(g),cal(c),sv(s){
	setDockNestingEnabled(true);
	
	setCentralWidget((QWidget*)glw);
	
	rendWg = new QDockWidget("Rendering",this);
	rendWg->setFeatures((QDockWidget::DockWidgetFeature)(QDockWidget::AllDockWidgetFeatures & ~QDockWidget::DockWidgetClosable));
	rendWg->setAllowedAreas(Qt::AllDockWidgetAreas);
	calcWg = new QDockWidget("Calculations",this);
	calcWg->setFeatures((QDockWidget::DockWidgetFeature)(QDockWidget::AllDockWidgetFeatures & ~QDockWidget::DockWidgetClosable));
	calcWg->setAllowedAreas(Qt::AllDockWidgetAreas);
	
	rend = new Ui::Rendering();
	rend->setupUi(rendWg);
	calc = new Ui::Calculations();
	calc->setupUi(calcWg);
	
	addDockWidget(Qt::RightDockWidgetArea, rendWg, Qt::Horizontal);
	addDockWidget(Qt::RightDockWidgetArea, calcWg, Qt::Horizontal);
	
	QObject::connect((const QObject*)sv, SIGNAL(fpsChanged(scalar,scalar,scalar,scalar)), 
		this, SLOT(fpsChanged(scalar,scalar,scalar,scalar)), Qt::QueuedConnection);
	QObject::connect(rend->x_rot, SIGNAL(valueChanged(double)), 
		this, SLOT(xAutoRot(double)), Qt::QueuedConnection);
	QObject::connect(rend->y_rot, SIGNAL(valueChanged(double)), 
		this, SLOT(yAutoRot(double)), Qt::QueuedConnection);
	QObject::connect(rend->z_rot, SIGNAL(valueChanged(double)), 
		this, SLOT(zAutoRot(double)), Qt::QueuedConnection);
	QObject::connect(rend->start, SIGNAL(clicked()), 
		cc cal, SLOT(start()), Qt::QueuedConnection);
	QObject::connect(rend->stop, SIGNAL(clicked()), 
		cc cal, SLOT(stop()), Qt::QueuedConnection);
	QObject::connect(rend->fps, SIGNAL(valueChanged(double)), 
		cc glw, SLOT(setRenderFps(double)), Qt::QueuedConnection);
	QObject::connect(rend->colours, SIGNAL(toggled(bool)), 
		this, SLOT(useColors(bool)), Qt::QueuedConnection);
	QObject::connect(rend->traces, SIGNAL(toggled(bool)), 
		this, SLOT(showTrace(bool)), Qt::QueuedConnection);
	QObject::connect(rend->connect_trace, SIGNAL(toggled(bool)), 
		this, SLOT(connectTrace(bool)), Qt::QueuedConnection);
	QObject::connect(calc->x, SIGNAL(valueChanged(double)), 
		this, SLOT(xBoxSize(double)), Qt::QueuedConnection);
	QObject::connect(calc->y, SIGNAL(valueChanged(double)), 
		this, SLOT(yBoxSize(double)), Qt::QueuedConnection);
	QObject::connect(calc->z, SIGNAL(valueChanged(double)), 
		this, SLOT(zBoxSize(double)), Qt::QueuedConnection);
	QObject::connect(rend->calc_speed, SIGNAL(valueChanged(double)), 
		this, SLOT(speedChanged(double)), Qt::QueuedConnection);
	
	rend->calc_speed->setValue(speed);
	calc->count->setValue(circlesCount);
	calc->radius_min->setValue(sphereSize.s[0]);
	calc->radius_max->setValue(sphereSize.s[1]);
	if(sphereSize.s[0] == sphereSize.s[1]){
		calc->one_sphereSize->setChecked(true);
	}else{
		calc->one_sphereSize->setChecked(false);
	}
	calc->x->setValue(boxSize.s[0]);
	calc->y->setValue(boxSize.s[1]);
	calc->z->setValue(boxSize.s[2]);
	calc->earth_gravity->setValue(gravity.s[0]);
	calc->inter_gravity->setValue(G_fact);
	calc->air_resistance->setValue(airResistance);
	calc->wall_resistance->setChecked(wallResistance);
	calc->e_modul->setValue(E);
	calc->poissons_ratio->setValue(poisson);
	calc->elasticity->setValue(elastic);
	rend->fps->setValue(renderFpsMax);
	rend->colours->setChecked(useColorsBool);
	rend->traces->setChecked(useTrace);
	rend->connect_trace->setChecked(connectTracePoints);
	rend->x_rot->setValue(autoRotation.s[0]);
	rend->y_rot->setValue(autoRotation.s[1]);
	rend->z_rot->setValue(autoRotation.s[2]);
}

void Control::fpsChanged(scalar glFps, scalar calFps, scalar fbLoad, scalar realSpeed){
	rend->render_fps->setValue(glFps);
	rend->calc_fps->setValue(calFps);
	rend->frame_buffer->setValue(fbLoad*100);
	rend->real_speed->setValue(realSpeed);
	rend->real_fps->setValue(calFps/realSpeed);
}

void Control::xAutoRot(double angle){
	autoRotation.s[0] = angle;
}
void Control::yAutoRot(double angle){
	autoRotation.s[1] = angle;
}
void Control::zAutoRot(double angle){
	autoRotation.s[2] = angle;
}
void Control::xBoxSize(double angle){
	boxSize.s[0] = angle;
	emit cal->boxSizeChanged();
}
void Control::yBoxSize(double angle){
	boxSize.s[1] = angle;
	emit cal->boxSizeChanged();
}
void Control::zBoxSize(double angle){
	boxSize.s[2] = angle;
	emit cal->boxSizeChanged();
}
void Control::useColors(bool b){
	useColorsBool = b;
}
void Control::showTrace(bool b){
	useTrace = b;
	if(b == false){
		if(cal!=NULL && cal->ceBuffer!=NULL)
			for(int i = 0; i<circlesCount; i++){
				cal->ceBuffer[i].traceFull = false;
				cal->ceBuffer[i].traceCount = 0;
			}
	}
}
void Control::connectTrace(bool b){
	connectTracePoints = b;
}
void Control::speedChanged(double d){
	speed = d;
}
