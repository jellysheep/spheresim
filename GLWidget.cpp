#include <QtGui/QMouseEvent>
#include "GLWidget.h"
#include <GL/glu.h>
#include <QTimer>
#include <QtGui/QApplication>
#include "Circles.h"

GLWidget::GLWidget(ClTimer* ct, QWidget *parent) : QGLWidget(parent) {
	//setMouseTracking(true);
	clTimer = ct;
	rotation = (vector3){0,0,0};
	xRot = yRot = zRot = 0;
	translateZ = 0;
	newFrame = false;
	//*
	rotationTimer = new QTimer(this);
	rotationTimer->setInterval(1000/renderFps);
	QObject::connect(rotationTimer, SIGNAL(timeout()), this, SLOT(timeToRender()), Qt::QueuedConnection);
	rotationTimer->start();// */
	QObject::connect(this, SIGNAL(timeToRender_()), this, SLOT(timeToRender()), Qt::DirectConnection);
}

void GLWidget::updateTimer() {
	if(!clTimer->newFrame) return;
	//printf("rotationTimer\n");
	QGLWidget::update();
	//printf(".\n");
}

void GLWidget::initializeGL() {
	/*glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);*/
	
	/*
	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glShadeModel( GL_SMOOTH );//flat/smooth
	//glShadeModel( GL_FLAT );
	glDepthMask(GL_TRUE);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.f, 1.f*800/600*boxSize.s0/boxSize.s1, 1.f, 3000.f);
	
	if(_3D_!=0){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
	}
	GLfloat LightAmbient[]= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { (float)boxSize.s0/2, (float)boxSize.s1, 0, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse); 
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);*/
	
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1,1,1,1);
	glPointSize(2);
	glLineWidth(1);
	
	// Set color and depth clear value
	glClearDepth(1.f);
	//glClearColor(0.f, 0.f, 0.f, 0.f);

	// Enable Z-buffer read and write
	glShadeModel( GL_SMOOTH );//flat/smooth
	//glShadeModel( GL_FLAT );

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.f, 1.f*boxSize.s1/boxSize.s0, 1.f, 2000.f);
	
	
	if(_3D_!=0){
		glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
	}
	GLfloat LightAmbient[]= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { (float)boxSize.s0/2, (float)boxSize.s1, (float)boxSize.s2/2, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse); 
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
}

void GLWidget::resizeGL(int w, int h) {
	//*
	if(boxSize.s0*h/boxSize.s1 > w){
		glViewport(0, 0, w, boxSize.s1*w/boxSize.s0);
	}else{
		glViewport(0, 0, boxSize.s0*h/boxSize.s1, h);
	}
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h); // set origin to bottom left corner
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();// */
}

void GLWidget::timeToRender(){
	if(!clTimer->newFrame) return;
	clTimer->newFrame = false;
	
	newFrame = true;
	//*
	int ms = 1;
	struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);// */
	//glDraw();
	update();
    //QCoreApplication::processEvents();
	drawingFinished = true;
}

void GLWidget::paintGL() {
	//makeCurrent();
	//printf("paintGL\n");
	/*
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW);
	//glTranslatef(-boxSize.s0/2, -boxSize.s1/2, -boxSize.s2/2);
	//glTranslatef(0.0, 0.0, -10.0);
	glTranslatef(0.0, 0.0, zRot/16.0);
	//glTranslatef(0.f, 0.f, 1000.f);
	//glRotatef(rotation.s0 / 16.0, 1.0, 0.0, 0.0);
	//glRotatef(rotation.s1 / 16.0, 0.0, 1.0, 0.0);
	//glRotatef(rotation.s2 / 16.0, 0.0, 0.0, 1.0);
	*/
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Apply some transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.f, 0.f, -500.f);
	glTranslatef(0.0, 0.0, zRot*1.0);

	//glRotatef(180.0, 0.0, 1.0, 0.0);
	glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);

	scalar scale = 300.0/boxSize.s0;
	#if !_3D_
		scale*=1.3;
	#endif
	glScalef(scale,scale,scale);

	//glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
	glTranslatef(-boxSize.s0/2, -boxSize.s1/2, (_3D_!=0?(-boxSize.s2/2):0));
	//glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
	
	clTimer->paintGL(rotation, translateZ);
	/*
	glColor3f(1,0,0);
	glBegin(GL_POLYGON);
	glVertex2f(0,0);
	glVertex2f(100,500);
	glVertex2f(500,100);
	glEnd(); // */
	frameCounter++;
	//doneCurrent();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
 {
     lastPos = event->pos();
 }

 void GLWidget::mouseMoveEvent(QMouseEvent *event)
 {
     int dx = event->x() - lastPos.x();
     int dy = event->y() - lastPos.y();

     if (event->buttons() & Qt::LeftButton) {
         setXRotation(xRot + 8 * dy);
         setYRotation(yRot + 8 * dx);
     } else if (event->buttons() & Qt::RightButton) {
         setXRotation(xRot + 8 * dy);
         setZRotation(zRot + 8 * dx);
     }
     lastPos = event->pos();
 }

void GLWidget::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
	case Qt::Key_Escape:
		close();
		break;
	default:
		event->ignore();
		break;
	}
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(100, 100);
}

QSize GLWidget::sizeHint() const
{
	return QSize(600, 800);
}

 static void qNormalizeAngle(int &angle)
 {
     while (angle < 0)
         angle += 360 * 16;
     while (angle > 360 * 16)
         angle -= 360 * 16;
 }

 void GLWidget::setXRotation(int angle)
 {
     qNormalizeAngle(angle);
     if (angle != xRot) {
         xRot = angle;
         emit xRotationChanged(angle);
         updateGL();
     }
 }

 void GLWidget::setYRotation(int angle)
 {
     qNormalizeAngle(angle);
     if (angle != yRot) {
         yRot = angle;
         emit yRotationChanged(angle);
         updateGL();
     }
 }

 void GLWidget::setZRotation(int angle)
 {
     qNormalizeAngle(angle);
     if (angle != zRot) {
         zRot = angle;
         emit zRotationChanged(angle);
         updateGL();
     }
 }
