#include "Ui_Control.h"
#include "Circles.h"
#include "Calculator.h"
#include "GLWidget.h"
//#include <QtGui>
#include <QtGui/QDockWidget>
#include "PlotWidget.h"

#define cc (const QObject*)
//const-cast
 

Control::Control(GLWidget* g, Calculator* c, StatusViewer* s):QMainWindow(),glw(g),cal(c),sv(s){
	setDockNestingEnabled(true);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	
	setCentralWidget((QWidget*)glw);
	
	rendWg = new QDockWidget("Rendering",this);
	rendWg->setFeatures((QDockWidget::DockWidgetFeature)(QDockWidget::AllDockWidgetFeatures & ~QDockWidget::DockWidgetClosable));
	rendWg->setAllowedAreas(Qt::AllDockWidgetAreas);
	calcWg = new QDockWidget("Calculations",this);
	calcWg->setFeatures((QDockWidget::DockWidgetFeature)(QDockWidget::AllDockWidgetFeatures & ~QDockWidget::DockWidgetClosable));
	calcWg->setAllowedAreas(Qt::AllDockWidgetAreas);
	graphWg = new PlotWidget("Wall force graph",this);
	graphWg->setFeatures((QDockWidget::DockWidgetFeature)(QDockWidget::AllDockWidgetFeatures & ~QDockWidget::DockWidgetClosable));
	graphWg->setAllowedAreas(Qt::AllDockWidgetAreas);
	
	rend = new Ui::Rendering();
	rend->setupUi(rendWg);
	calc = new Ui::Calculations();
	calc->setupUi(calcWg);
	
	QSize rendSize = rendWg->size();
	rendSize.setHeight(5000);
	rendWg->setMaximumSize(rendSize);
	QSize calcSize = calcWg->size();
	calcSize.setHeight(5000);
	calcWg->setMaximumSize(calcSize);
	
	addDockWidget(Qt::RightDockWidgetArea, calcWg, Qt::Horizontal);
	addDockWidget(Qt::RightDockWidgetArea, rendWg, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, graphWg, Qt::Horizontal);
	
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
		
	QObject::connect(calc->count, SIGNAL(valueChanged(int)), 
		cal, SLOT(circleCountChanged(int)), Qt::QueuedConnection);
		
	QObject::connect(calc->wireframe, SIGNAL(toggled(bool)), 
		this, SLOT(showWireframe(bool)), Qt::QueuedConnection);
	QObject::connect(rend->reflections, SIGNAL(toggled(bool)), 
		this, SLOT(showReflections(bool)), Qt::QueuedConnection);
	
	QObject::connect(rend->reset_view, SIGNAL(clicked()), 
		cc glw, SLOT(resetView()), Qt::QueuedConnection);
		
	QObject::connect(calc->one_size, SIGNAL(toggled(bool)), 
		this, SLOT(oneSphereSize(bool)), Qt::QueuedConnection);
	QObject::connect(calc->radius_max, SIGNAL(valueChanged(double)), 
		this, SLOT(maxSphereSize(double)), Qt::QueuedConnection);
	QObject::connect(calc->radius_min, SIGNAL(valueChanged(double)), 
		this, SLOT(minSphereSize(double)), Qt::QueuedConnection);
	QObject::connect(calc->visible_count, SIGNAL(valueChanged(int)), 
		cal, SLOT(maxCircleCountChanged(int)), Qt::QueuedConnection);
	QObject::connect(calc->cube, SIGNAL(toggled(bool)), 
		this, SLOT(setShowCube(bool)), Qt::QueuedConnection);
	QObject::connect(calc->earth_gravity, SIGNAL(valueChanged(double)), 
		this, SLOT(earthGravity(double)), Qt::QueuedConnection);
	QObject::connect(calc->inter_gravity, SIGNAL(valueChanged(double)), 
		this, SLOT(interGravity(double)), Qt::QueuedConnection);
	QObject::connect(calc->air_resistance, SIGNAL(valueChanged(double)), 
		this, SLOT(setAirResistance(double)), Qt::QueuedConnection);
	QObject::connect(calc->wall_resistance, SIGNAL(toggled(bool)), 
		this, SLOT(setWallResistance(bool)), Qt::QueuedConnection);
	QObject::connect(calc->e_modul, SIGNAL(valueChanged(double)), 
		this, SLOT(setEModul(double)), Qt::QueuedConnection);
	QObject::connect(calc->poissons_ratio, SIGNAL(valueChanged(double)), 
		this, SLOT(setPoisson(double)), Qt::QueuedConnection);
	QObject::connect(calc->elasticity, SIGNAL(valueChanged(double)), 
		this, SLOT(setElasticity(double)), Qt::QueuedConnection);
	QObject::connect(rend->hue_rot, SIGNAL(valueChanged(double)), 
		this, SLOT(setHueStep(double)), Qt::QueuedConnection);
	QObject::connect(rend->lights, SIGNAL(toggled(bool)), 
		this, SLOT(setShowLights(bool)), Qt::QueuedConnection);
	QObject::connect(rend->trace_length, SIGNAL(valueChanged(double)), 
		this, SLOT(setTraceAmount(double)), Qt::QueuedConnection);
	
	rend->calc_speed->setValue(speed);
	calc->count->setValue(circlesCount);
	calc->visible_count->setValue(maxShowCirclesCount);
	calc->radius_min->setValue(sphereSize.s[0]);
	calc->radius_max->setValue(sphereSize.s[1]);
	calc->one_size->setChecked(sphereSize.s[0] == sphereSize.s[1]);
	calc->x->setValue(boxSize.s[0]);
	calc->y->setValue(boxSize.s[1]);
	calc->z->setValue(boxSize.s[2]);
	calc->earth_gravity->setValue(gravity_abs);
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
	calc->wireframe->setChecked(wireframe);
	rend->reflections->setChecked(reflections);
	calc->cube->setChecked(showCube);
	rend->hue_rot->setValue(hueStep);
	rend->lights->setChecked(showLights);
	rend->trace_length->setValue(traceAmount);
}

void Control::setTraceAmount(double d){
	traceAmount = d;
}

void Control::setShowLights(bool b){
	showLights = b;
}

void Control::setHueStep(double d){
	hueStep = d;
}

void Control::setElasticity(double d){
	elastic = d;
	cal->updateElasticity();
}

void Control::setPoisson(double d){
	poisson = d;
	cal->updatePoisson();
}

void Control::setEModul(double d){
	E = d;
	cal->updateEModul();
}

void Control::setWallResistance(bool b){
	wallResistance = b;
	cal->updateWallResistance();
}

void Control::setAirResistance(double a){
	airResistance = a;
	cal->updateAirResistance();
}

void Control::interGravity(double g){
	G_fact = g;
	cal->updateG();
}

void Control::earthGravity(double g){
	gravity_abs = g;
	glw->updateGravity();
}

void Control::oneSphereSize(bool b){
	if(b == true){
		sphereSize.s[1] = sphereSize.s[0];
		calc->radius_max->setValue(sphereSize.s[0]);
	}
}
void Control::minSphereSize(double s){
	sphereSize.s[0] = s;
}
void Control::maxSphereSize(double s){
	sphereSize.s[1] = s;
}

void Control::updateGL(){
	emit glw->timeToRender2();
}

void Control::keyPressEvent(QKeyEvent* event){
	if(event->isAutoRepeat()){
		event->ignore();
		return;
	}
	switch(event->key()){
		case Qt::Key_Space:
			emit this->toggleRunning();
			break;
		case Qt::Key_Escape:
			break;
		case Qt::Key_H:
			calcWg->setHidden(!calcWg->isHidden());
			rendWg->setHidden(!rendWg->isHidden());
			break;
		case Qt::Key_F:
			if(fullscreen){
				showNormal();
				showMaximized();
			} else showFullScreen();
			break;
		case Qt::Key_R:
			emit glw->resetView();
			break;
		default:
			event->ignore();
			break;
	}
}

void Control::showMaximized(){
	fullscreen = false;
	QMainWindow::showMaximized();
}

void Control::showFullScreen(){
	fullscreen = true;
	QMainWindow::showFullScreen();
}

void Control::showNormal(){
	fullscreen = false;
	QMainWindow::showNormal();
}

void Control::toggleRunning(){
	if(cal->isRunning()){
		cal->stop();
		rend->stop->setEnabled(false);
		rend->start->setEnabled(true);
	}else{
		cal->start();
		rend->stop->setEnabled(true);
		rend->start->setEnabled(false);
	}
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
	updateGL();
}
void Control::yAutoRot(double angle){
	autoRotation.s[1] = angle;
	updateGL();
}
void Control::zAutoRot(double angle){
	autoRotation.s[2] = angle;
	updateGL();
}
void Control::xBoxSize(double angle){
	boxSize.s[0] = angle;
	emit cal->boxSizeChanged();
	updateGL();
}
void Control::yBoxSize(double angle){
	boxSize.s[1] = angle;
	emit cal->boxSizeChanged();
	updateGL();
}
void Control::zBoxSize(double angle){
	boxSize.s[2] = angle;
	emit cal->boxSizeChanged();
	updateGL();
}
void Control::useColors(bool b){
	useColorsBool = b;
	updateGL();
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
	updateGL();
}
void Control::connectTrace(bool b){
	connectTracePoints = b;
	updateGL();
}
void Control::speedChanged(double d){
	speed = d;
}

void Control::showWireframe(bool b){
	wireframe = b;
	updateGL();
}
void Control::showReflections(bool b){
	reflections = b;
	updateGL();
}
void Control::setShowCube(bool b){
	showCube = b;
	updateGL();
}
