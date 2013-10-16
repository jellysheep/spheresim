/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <GLWidget.hpp>
#include <Console.hpp>

#include <QTimer>
#include <GL/glu.h>

using namespace SphereSim;

GLWidget::GLWidget(QWidget* parent):QGLWidget(parent)
{
	animationTimer = new QTimer(this);
	connect(animationTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
	animationTimer->start(1000/60);
}

GLWidget::~GLWidget()
{
	animationTimer->stop();
	delete animationTimer;
}

void GLWidget::initializeGL()
{
	qglClearColor(QColor("lightgray"));
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1,1,1,1);
	
	glClearDepth(1.f);

	glShadeModel(GL_SMOOTH);

	glLoadIdentity();
	
	glPointSize(1.5);
}

void GLWidget::resizeGL(int width, int height)
{
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.f, 1.f*width/height, 0.01f, 5000.f);
	glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	paintBackground();
}

void GLWidget::paintBackground()
{
	glPushMatrix();
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);
	float f = -1, col, x=5, y=0.47;
	col = 0.99;
	glColor3f(col,col,col);
	glVertex3f(-x, y, f);
	glVertex3f( x, y, f);
	col = 0.7;
	glColor3f(col,col,col);
	glVertex3f( x,-y, f);
	glVertex3f(-x,-y, f);
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glPopMatrix();
}
