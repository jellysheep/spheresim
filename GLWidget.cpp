#include <QtGui/QMouseEvent>
#include "GLWidget.h"
#include <GL/glu.h>
#include <QTimer>
#include "Circles.h"

GLWidget::GLWidget(ClTimer* ct, QWidget *parent) : QGLWidget(parent) {
	//setMouseTracking(true);
	clTimer = ct;
	rotation = (cl_double3){0,0,0,0};
	translateZ = 0;
	QTimer* rotationTimer = new QTimer(this);
	rotationTimer->setInterval(1000/60);
	QObject::connect(rotationTimer, SIGNAL(timeout()), this, SLOT(updateTimer()), Qt::QueuedConnection);
	rotationTimer->start();
}

void GLWidget::updateTimer() {
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
	gluPerspective(50.f, 1.f*480/360*boxSize.s0/boxSize.s1, 1.f, 2000.f);
	
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
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
}

void GLWidget::reboxSizeGL(int w, int h) {
	/*glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h); // set origin to bottom left corner
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/
}

void GLWidget::paintGL() {
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	clTimer->paintGL(rotation, translateZ);
	glColor3f(1,0,0);
	glBegin(GL_POLYGON);
	glVertex2f(0,0);
	glVertex2f(100,500);
	glVertex2f(500,100);
	glEnd();
	frameCounter++;
}

void GLWidget::mousePressEvent(QMouseEvent *event) {

}
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	printf("%d, %d\n", event->x(), event->y());
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
