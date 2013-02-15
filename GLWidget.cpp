#include <QtGui/QMouseEvent>
#include "GLWidget.h"
#include <GL/glu.h>
#include <QTimer>
#include <QtGui/QApplication>
#include <cmath>
#include "Calculator.h"

GLWidget::GLWidget(Calculator* ct, QWidget *parent) : QGLWidget(parent) {
	setFocusPolicy(Qt::StrongFocus);
	//setMouseTracking(true);
	clTimer = ct;
	rotation = (vector3){0,0,0};
	#if _3D_
		xRot = 40;
		yRot = 60;
	#else
		xRot = 0;
		yRot = 0;
	#endif
	zRot = 0;
	xRotCam = 0;
	yRotCam = 0;
	transX = 0;
	transY = 0;
	transZ = 0;
	translate = 0;
	newFrame = false;
	rotGrav = 0;
	
	
	//*
	rotationTimer = new QTimer(this);
	rotationTimer->setInterval(1000/(renderFps-5));
	QObject::connect(rotationTimer, SIGNAL(timeout()), this, SLOT(timeToRender()), Qt::QueuedConnection);
	rotationTimer->start();// */
	QObject::connect(this, SIGNAL(timeToRender_()), this, SLOT(timeToRender()), Qt::DirectConnection);
}

void GLWidget::updateTimer() {
	if(!clTimer->getNewFrame()) return;
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
	gluPerspective(50.f, 1.f*800/600*boxSize.s[0]/boxSize.s[1], 1.f, 3000.f);
	
	if(_3D_!=0){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
	}
	GLfloat LightAmbient[]= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { (float)boxSize.s[0]/2, (float)boxSize.s[1], 0, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse); 
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);*/
	
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	//~ glEnable(GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1,1,1,1);
	
	// Set color and depth clear value
	glClearDepth(1.f);
	//~ glClearColor(0.f, 0.f, 0.f, 0.5f);

	// Enable Z-buffer read and write
	glShadeModel( GL_SMOOTH );//flat/smooth
	//~ glShadeModel( GL_FLAT );

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.f, 1.f*boxSize.s[1]/boxSize.s[0], 1.f, 2000.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	
	#if _3D_
		glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		
		glEnable(GL_LIGHTING);
		
		float light;
		
		GLfloat mat_shininess[] = { 100.0 };
		light = 0.6f;
		GLfloat LightSpecular[]= { light, light, light, 1.0f };
		glMaterialfv(GL_FRONT, GL_SPECULAR, LightSpecular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		
		light = 0.05f;
		GLfloat LightAmbient[]= { light, light, light, 1.0f };
		light = 0.4f;
		GLfloat LightDiffuse[]= { light, light, light, 1.0f };
		LightPosition = new GLfloat*[4];
		int lights[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3};
		float fact = 1.5f;
		for(int i = 0; i<4; i++){
			LightPosition[i] = new GLfloat[4];
			for(int j = 0; j<3; j++){
				LightPosition[i][j] = boxSize.s[j]/2;
			}
			LightPosition[i][1] += (float)boxSize.s[1]/2*fact;
			LightPosition[i][3] = 1.0f;
		}
		fact = 2;
		LightPosition[0][0] += +(float)boxSize.s[0]/2*fact;
		LightPosition[0][2] += -(float)boxSize.s[2]/2*fact;
		LightPosition[1][0] += -(float)boxSize.s[0]/2*fact;
		LightPosition[1][2] += +(float)boxSize.s[2]/2*fact;
		LightPosition[2][0] += +(float)boxSize.s[0]/2*fact;
		LightPosition[2][2] += +(float)boxSize.s[2]/2*fact;
		LightPosition[3][0] += -(float)boxSize.s[0]/2*fact;
		LightPosition[3][2] += -(float)boxSize.s[2]/2*fact;
		for(int i = 0; i<4; i++){
			glLightfv(lights[i], GL_AMBIENT, LightAmbient);
			glLightfv(lights[i], GL_DIFFUSE, LightDiffuse); 
			glLightfv(lights[i], GL_POSITION,LightPosition[i]);
			if(i<2)glLightfv(lights[i], GL_SPECULAR, LightSpecular);
		}
		
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHT3);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		
		//glEnable(GL_NORMALIZE);
		glEnable(GL_RESCALE_NORMAL);
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	#endif
	
	displayList = glGenLists(2);
	glNewList(displayList,GL_COMPILE);
	#if _3D_
		drawsphere(2,1.f);
	#else
		drawCircleF(1.f);
	#endif
	glEndList();
	glNewList(displayList+1,GL_COMPILE);
	glLineWidth(1.0);
	glColor3d(0,0.5,0.3);
	glBegin(GL_LINE_LOOP);
	glVertex3d(0,0,0);
	glVertex3d(1,0,0);
	glVertex3d(1,1,0);
	glVertex3d(0,1,0);
	glEnd();
	if(_3D_!=0){
		//glColor3d(0.2,0.2,0.2);
		glBegin(GL_LINE_LOOP);
		glVertex3d(0,0,1);
		glVertex3d(1,0,1);
		glVertex3d(1,1,1);
		glVertex3d(0,1,1);
		glEnd();
		//glColor3d(0.2,0.2,0.2);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(0,0,1);
		glVertex3d(1,0,0);
		glVertex3d(1,0,1);
		glVertex3d(1,1,0);
		glVertex3d(1,1,1);
		glVertex3d(0,1,0);
		glVertex3d(0,1,1);
		glEnd();
		
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);  
		drawBoxSides();
		
		glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);
		drawBoxSides();
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}
	
	glEndList();
	glLineWidth(1.5);
	glPointSize(1.5);
}

void GLWidget::resizeGL(int w, int h) {
	/*
	if(boxSize.s[0]*h/boxSize.s[1] > w){
		glViewport(0, 0, w, boxSize.s[1]*w/boxSize.s[0]);
	}else{
		glViewport(0, 0, boxSize.s[0]*h/boxSize.s[1], h);
	}//*/
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.f, 1.f*w/h, 1.f, 2000.f);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h); // set origin to bottom left corner
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();// */
}

void GLWidget::timeToRender(){
	if(!clTimer->getNewFrame()) return;
	clTimer->setNewFrame(false);
	newFrame = true;
	
	//#if _3D_
		xRot += autoRotation.s[0]*16.0;
		yRot += autoRotation.s[1]*16.0;
		zRot += autoRotation.s[2]*16.0;
	//#endif
	
	/*
	rotGrav += 0.2;
	gravity.s[0] = cos(M_PI/180.0*(rotGrav+90))*gravity_abs;
	gravity.s[1] = sin(M_PI/180.0*(rotGrav+90))*gravity_abs;
	#if _3D_
		gravity.s[2] = 0;
	#endif
	emit clTimer->gravityChanged();
	//*/
	
	timeToRender2();
}

void GLWidget::timeToRender2(){
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
	//glTranslatef(-boxSize.s[0]/2, -boxSize.s[1]/2, -boxSize.s[2]/2);
	//glTranslatef(0.0, 0.0, -10.0);
	glTranslatef(0.0, 0.0, zRot/16.0);
	//glTranslatef(0.f, 0.f, 1000.f);
	//glRotatef(rotation.s[0] / 16.0, 1.0, 0.0, 0.0);
	//glRotatef(rotation.s[1] / 16.0, 0.0, 1.0, 0.0);
	//glRotatef(rotation.s[2] / 16.0, 0.0, 0.0, 1.0);
	*/
	// Clear color and depth buffer
	if(reflections){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}else{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	
	
	//*
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPushMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);
	float f = -1, col;
	col = 0.9;
	glColor3f(col,col,col);
	glVertex3f(-1.0, 1.0, f);
	glVertex3f(1.0,1.0, f);
	col = 0.1;
	glColor3f(col,col,col);
	glVertex3f(1.0,-1.0, f);
	glVertex3f(-1.0,-1.0, f);
	glEnd();
	#if _3D_
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	#endif
	
	glPopMatrix();//*/
	
	
	if(wireframe){
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}else{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	// Apply some transformations
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glRotatef(180.0, 0.0, 1.0, 0.0);
	glRotatef(xRotCam, 1.0, 0.0, 0.0);
	glRotatef(yRotCam, 0.0, 1.0, 0.0);
	
	
	glTranslatef(-transX, -transY, -transZ);
	
	glTranslatef(0.f, 0.f, -500.f);
	glTranslatef(0.f, 0.f, 280.f);
	
	
	glRotatef(xRot, 1.0, 0.0, 0.0);
	glRotatef(yRot, 0.0, 1.0, 0.0);
	glRotatef(zRot, 0.0, 0.0, 1.0);

	
	scalar scale = 80.0;
	#if !_3D_
		scale*=1.5;
	#endif
	glScalef(scale,scale,scale);

	
	
	
	
	glRotatef(-rotGrav, 0.0, 0.0, 1.0);
	//glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
	glTranslatef(-boxSize.s[0]/2, -boxSize.s[1]/2, (_3D_!=0?(-boxSize.s[2]/2):0));
	#if _3D_
		setLightPos();
		//draw light bulbs
		drawLights();
	#endif
	
	if(renderBool){
		#if _3D_
		if(reflections){
			reflect();
		}else
		#endif
		clTimer->paintGL(true);
		/* Draw "bottom" of floor in blue. */
		/* Switch face orientation. */
		//*
		
		/*
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);  
		drawBoxSides();
		glFrontFace(GL_CW);
		drawBoxSides();
		glDisable(GL_CULL_FACE);*/
	}
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

void GLWidget::setLightPos(){
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition[0]);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition[1]);
	glLightfv(GL_LIGHT2, GL_POSITION,LightPosition[2]);
	glLightfv(GL_LIGHT3, GL_POSITION,LightPosition[3]);
}

void GLWidget::drawBoxSides(){
	float opacity = 0.1;
	glColor4f(0.0, 0.0, 0.0, opacity);
	glBegin(GL_QUADS);
		double y = -0.01;
		//~ glColor4f(1,0,0,opacity);
		glNormal3d(0,-1,0);
		glVertex3d(y,y,y);
		glVertex3d(y,y,1-y);
		glVertex3d(1-y,y,1-y);
		glVertex3d(1-y,y,y);
		
		//~ glColor4f(0,1,0,opacity);
		glNormal3d(1,0,0);
		glVertex3d(1-y,y,y);
		glVertex3d(1-y,y,1-y);
		glVertex3d(1-y,1-y,1-y);
		glVertex3d(1-y,1-y,y);
		
		//~ glColor4f(0,0,1,opacity);
		glNormal3d(0,1,0);
		glVertex3d(1-y,1-y,y);
		glVertex3d(1-y,1-y,1-y);
		glVertex3d(y,1-y,1-y);
		glVertex3d(y,1-y,y);
		
		//~ glColor4f(1,1,0,opacity);
		glNormal3d(-1,0,0);
		glVertex3d(y,1-y,y);
		glVertex3d(y,1-y,1-y);
		glVertex3d(y,y,1-y);
		glVertex3d(y,y,y);
		
		//~ glColor4f(1,0,1,opacity);
		glNormal3d(0,0,-1);
		glVertex3d(y,y,y);
		glVertex3d(1-y,y,y);
		glVertex3d(1-y,1-y,y);
		glVertex3d(y,1-y,y);
		
		//~ glColor4f(0,1,1,opacity);
		glNormal3d(0,0,1);
		glVertex3d(y,y,1-y);
		glVertex3d(y,1-y,1-y);
		glVertex3d(1-y,1-y,1-y);
		glVertex3d(1-y,y,1-y);
	glEnd();
}

void GLWidget::drawQuad(int i){
	if(i == 0){
		glBegin(GL_QUADS);
		glVertex3d(0,0,0);
		glVertex3d(boxSize.s[0],0,0);
		glVertex3d(boxSize.s[0],0,boxSize.s[2]);
		glVertex3d(0,0,boxSize.s[2]);
		glEnd();
	}
}

void GLWidget::reflect(){
	static double reflection = 0.7;//0.95;
	for(int i = 0; i<1; i++){
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		
		/* Don't update color or depth. */
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		/* Draw 1 into the stencil buffer. */
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

		/* Now drawing the floor just tags the floor pixels
		as stencil value 1. */
		
		drawQuad(i);

		/* Re-enable update of color and depth. */ 
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		/* Now, only render where stencil is set to 1. */
		glStencilFunc(GL_EQUAL, 1, 0xffffffff);  /* draw if stencil ==1 */
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		/* Draw reflected ninja, but only where floor is. */
		glPushMatrix();
		glScalef(1.0, -1.0, 1.0);
		setLightPos();
		
		glCullFace(GL_FRONT);
		if(i == 0)
			clTimer->paintGL(true);
		else
			clTimer->paintGL(false);
		glCullFace(GL_BACK);
		glPopMatrix();
		setLightPos();

		glDisable(GL_STENCIL_TEST);
		
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glColor4f(1.0, 1.0, 1.0, reflection);
		drawQuad(i);
		
		glCullFace(GL_FRONT);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		drawQuad(i);
		
		// */
		glDisable(GL_CULL_FACE);
	}
	
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	/* Don't update color or depth. */
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	/* Draw 1 into the stencil buffer. */
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

	/* Now drawing the floor just tags the floor pixels
	as stencil value 1. */
	for(int i = 0; i<1; i++){
		
		drawQuad(i);
	}
	glDisable(GL_CULL_FACE);

	/* Re-enable update of color and depth. */ 
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	/* Now, only render where stencil is set to 1. */
	glStencilFunc(GL_NOTEQUAL, 1, 0xffffffff);  /* draw if stencil ==1 */
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	clTimer->paintGL(false);
	glDisable(GL_STENCIL_TEST);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void shrinkAngle(scalar& s){
	while(s>360)	s-=360;
	while(s<0)		s+=360;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if ((event->buttons() & Qt::LeftButton)&&(event->modifiers() & Qt::ShiftModifier)) {
		xRotCam += 8/16.0 * dy;
		yRotCam += 8/16.0 * dx;
		shrinkAngle(xRotCam);
		shrinkAngle(yRotCam);
	} else if (event->buttons() & Qt::LeftButton) {
		setXRotation(xRot + 8/16.0 * dy);
		setYRotation(yRot + 8/16.0 * dx);
	} else if (event->buttons() & Qt::RightButton) {
		//translate
		translate = dx;
		transX += translate*sin(yRotCam*M_PI/180.0)*cos(xRotCam*M_PI/180.0);
		transY -= translate*sin(xRotCam*M_PI/180.0);
		transZ -= translate*cos(yRotCam*M_PI/180.0)*cos(xRotCam*M_PI/180.0);
	} else if (event->buttons() & Qt::MiddleButton) {
		transX += -dx*0.2*cos(yRotCam*M_PI/180.0);
		transZ += -dx*0.2*sin(yRotCam*M_PI/180.0);
		transY += dy*0.2;
	}
	lastPos = event->pos();
	timeToRender2();
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
		case Qt::Key_Escape:
			break;
		default:
			event->ignore();
			break;
	}
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(300, 300);
}

QSize GLWidget::sizeHint() const
{
	return QSize(3000, 3000);
}

static void qNormalizeAngle(scalar &angle)
{
	while (angle < 0)
		angle += 360;
	while (angle > 360)
		angle -= 360;
}

void GLWidget::setXRotation(scalar angle)
{
	qNormalizeAngle(angle);
	if (angle != xRot) {
		xRot = angle;
	}
}

void GLWidget::setYRotation(scalar angle)
{
	qNormalizeAngle(angle);
	if (angle != yRot) {
		yRot = angle;
	}
}

void GLWidget::setRenderFps(double fps){
	renderFps = fps;
	rotationTimer->setInterval(1000/renderFps);
}
 
#define _X .525731112119133606 
#define _Z .850650808352039932

static float vdata[12][3] = {	
	{-_X, 0.0, _Z}, {_X, 0.0, _Z}, {-_X, 0.0, -_Z}, {_X, 0.0, -_Z},	
	{0.0, _Z, _X}, {0.0, _Z, -_X}, {0.0, -_Z, _X}, {0.0, -_Z, -_X},	
	{_Z, _X, 0.0}, {-_Z, _X, 0.0}, {_Z, -_X, 0.0}, {-_Z, -_X, 0.0} 
};
static uint tindices[20][3] = { 
	{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},	
	{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},	
	{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
	{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

void GLWidget::normalize(GLfloat *a) {
	GLfloat d=sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
	a[0]/=d; a[1]/=d; a[2]/=d;
}

void GLWidget::drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r) {
	if (div<=0) {
		glNormal3fv(a); glVertex3f(a[0]*r, a[1]*r, a[2]*r);
		glNormal3fv(b); glVertex3f(b[0]*r, b[1]*r, b[2]*r);
		glNormal3fv(c); glVertex3f(c[0]*r, c[1]*r, c[2]*r);
	} else {
		GLfloat ab[3], ac[3], bc[3];
		for (int i=0;i<3;i++) {
			ab[i]=(a[i]+b[i])/2;
			ac[i]=(a[i]+c[i])/2;
			bc[i]=(b[i]+c[i])/2;
		}
		normalize(ab); normalize(ac); normalize(bc);
		drawtri(a, ab, ac, div-1, r);
		drawtri(b, bc, ab, div-1, r);
		drawtri(c, ac, bc, div-1, r);
		drawtri(ab, bc, ac, div-1, r);  //<--Comment this line and sphere looks really cool!
	}  
}

void GLWidget::drawsphere(int ndiv, float radius=1.0) {
	glBegin(GL_TRIANGLES);
	for (int i=0;i<20;i++)
		drawtri(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], ndiv, radius);
	glEnd();
}

void GLWidget::drawCircleF2(float r){
	//glScalef(r,r,r);
	glCallList(displayList);
}

void GLWidget::drawsphere2(float radius) {
	//glScalef(r,r,r);
	glCallList(displayList);
}

void GLWidget::drawCircleF(float r){
	static float d;
	static int j;
	d = 0;
	for(j = 0; j<=edges; j++){
		d+=step;
		//cout<<edges<<" "<<j<<endl;
		//cout<<d<<endl;
		glVertex3d(cos(d)*r,sin(d)*r,0);
	}
	glEnd();
}

void GLWidget::drawLights(){
	for(int i = 0; i<4; i++){
		glPushMatrix();
		glColor3d(1,1,0); 
		glTranslated(LightPosition[i][0], LightPosition[i][1], LightPosition[i][2]);
		float scale = 0.02;
		glScalef(scale,scale,scale);
		//glWidget->drawsphere(1,r);
		drawsphere2(0.1);
		//glTranslated(-x,-y,-z);
		glPopMatrix();
	}
}

void drawCircleF_old(float r){
	static float d;
	static int j;
	glBegin(GL_TRIANGLE_FAN);
	glPushAttrib(GL_CURRENT_BIT);
	d = 0;
	#if onlyOneC
		if(i==cCount-1)
			glColor3d(1,1,1); 
		else
			glColor3d(0.1, 0.1, 0.1); 
	#else
		glColor3d(1,1,1); 
	#endif
	glVertex3d((r/3.f),(r/3.f),0);
	glColor3d(0,1,0); 
	glPopAttrib();
	for(j = 0; j<=edges; j++){
		d+=step;
		//cout<<edges<<" "<<j<<endl;
		//cout<<d<<endl;
		glVertex3d(cos(d)*r,sin(d)*r,0);
	}
	glEnd();
}
