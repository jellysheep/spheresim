#include "Ui_Control.h"
#include "Circles.h"
#include "Calculator.h"

#define cc (const QObject*)
//const-cast

Control::Control(GLWidget* g, Calculator* c, StatusViewer* s):glw(g),cal(c),sv(s){
}

void Control::setupUi(QWidget* w){
	Ui::Control::setupUi(w);
	
	QObject::connect((const QObject*)sv, SIGNAL(fpsChanged(scalar,scalar,scalar,scalar)), 
		this, SLOT(fpsChanged(scalar,scalar,scalar,scalar)), Qt::QueuedConnection);
	QObject::connect(x_rot, SIGNAL(valueChanged(double)), 
		this, SLOT(xAutoRot(double)), Qt::QueuedConnection);
	QObject::connect(y_rot, SIGNAL(valueChanged(double)), 
		this, SLOT(yAutoRot(double)), Qt::QueuedConnection);
	QObject::connect(z_rot, SIGNAL(valueChanged(double)), 
		this, SLOT(zAutoRot(double)), Qt::QueuedConnection);
	QObject::connect(start, SIGNAL(clicked()), 
		cc cal, SLOT(start()), Qt::QueuedConnection);
	QObject::connect(stop, SIGNAL(clicked()), 
		cc cal, SLOT(stop()), Qt::QueuedConnection);
	QObject::connect(fps, SIGNAL(valueChanged(double)), 
		cc glw, SLOT(setRenderFps(double)), Qt::QueuedConnection);
	QObject::connect(colors, SIGNAL(toggled(bool)), 
		this, SLOT(useColors(bool)), Qt::QueuedConnection);
	QObject::connect(traces, SIGNAL(toggled(bool)), 
		this, SLOT(showTrace(bool)), Qt::QueuedConnection);
	QObject::connect(connect_trace, SIGNAL(toggled(bool)), 
		this, SLOT(connectTrace(bool)), Qt::QueuedConnection);
	QObject::connect(x, SIGNAL(valueChanged(double)), 
		this, SLOT(xBoxSize(double)), Qt::QueuedConnection);
	QObject::connect(y, SIGNAL(valueChanged(double)), 
		this, SLOT(yBoxSize(double)), Qt::QueuedConnection);
	QObject::connect(z, SIGNAL(valueChanged(double)), 
		this, SLOT(zBoxSize(double)), Qt::QueuedConnection);
	QObject::connect(calc_speed, SIGNAL(valueChanged(double)), 
		this, SLOT(speedChanged(double)), Qt::QueuedConnection);
	
	calc_speed->setValue(speed);
	count->setValue(circlesCount);
	radius_min->setValue(size.s[0]);
	radius_max->setValue(size.s[1]);
	if(size.s[0] == size.s[1]){
		one_size->setChecked(true);
	}else{
		one_size->setChecked(false);
	}
	x->setValue(boxSize.s[0]);
	y->setValue(boxSize.s[1]);
	z->setValue(boxSize.s[2]);
	earth_gravity->setValue(gravity.s[0]);
	inter_gravity->setValue(G_fact);
	air_resistance->setValue(airResistance);
	wall_resistance->setChecked(wallResistance);
	e_modul->setValue(E);
	poissons_ratio->setValue(poisson);
	elasticity->setValue(elastic);
	fps->setValue(renderFpsMax);
	colors->setChecked(useColorsBool);
	traces->setChecked(useTrace);
	connect_trace->setChecked(connectTracePoints);
	x_rot->setValue(autoRotation.s[0]);
	y_rot->setValue(autoRotation.s[1]);
	z_rot->setValue(autoRotation.s[2]);
}

void Control::fpsChanged(scalar glFps, scalar calFps, scalar fbLoad, scalar realSpeed){
	render_fps->setValue(glFps);
	calc_fps->setValue(calFps);
	frame_buffer->setValue(fbLoad*100);
	real_speed->setValue(realSpeed);
	real_fps->setValue(calFps/realSpeed);
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
