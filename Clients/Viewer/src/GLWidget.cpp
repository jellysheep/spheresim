/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <GLWidget.hpp>
#include <Console.hpp>

#include <QDebug>
#include <QtOpenGL>
#include <QtGui>
#include <QTimer>
#include <QElapsedTimer>

using namespace SphereSim;

GLWidget::GLWidget(QWidget* parent):QGLWidget(parent), program(this), circleEdges(50)
{
	frameBuffer = NULL;
	sleepTime = 20;
	animating = false;
	frameBufferPercentageLevelSum = 0;
	frameBufferPercentageLevelCounter = 0;
	animationTimer = new QTimer();
	connect(animationTimer, SIGNAL(timeout()), SLOT(timerUpdate()));
	animationTimer->setTimerType(Qt::PreciseTimer);
	animationTimer->setSingleShot(true);
	controlTimer = new QElapsedTimer();
	verticesAttr = 0;
	colorsAttr = 0;
	worldMatrixUniform = 0;
	sphereMatrixUniform = 0;
	circleVertices = NULL;
	circleColors = NULL;
}

void GLWidget::setFrameBuffer(FrameBuffer<Sphere>* fb)
{
	frameBuffer = fb;
}

GLWidget::~GLWidget()
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	program.release();
	delete animationTimer;
	delete controlTimer;
}

void GLWidget::initializeGL()
{
	initializeGLFunctions();
	qglClearColor(Qt::black);
	
	if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/VertexShader.glsl"))
		shaderLoadError();
	if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/FragmentShader.glsl"))
		shaderLoadError();
	if (!program.link())
		shaderLoadError();
	if (!program.bind())
		shaderLoadError();
	
	program.bind();
	
	verticesAttr = program.attributeLocation("verticesAttr");
	colorsAttr = program.attributeLocation("colorsAttr");
	worldMatrixUniform = program.uniformLocation("worldMatrix");
	sphereMatrixUniform = program.uniformLocation("sphereMatrix");
	
	circleVertices = new float[2*(circleEdges+2)];
	circleVertices[0] = -0.15;
	circleVertices[1] = 0.15;
	circleColors = new float[3*(circleEdges+2)];
	circleColors[0] = 1;
	circleColors[1] = 1;
	circleColors[2] = 1;
	float angle;
	for(quint16 i = 1; i<circleEdges+2; i++)
	{
		angle = 2*M_PI*(i-1)/circleEdges;
		circleVertices[2*i] = cos(angle);
		circleVertices[2*i+1] = sin(angle);
		circleColors[3*i] = 0.2;
		circleColors[3*i+1] = 0.2;
		circleColors[3*i+2] = 0.2;
	}

	glVertexAttribPointer(verticesAttr, 2, GL_FLOAT, GL_FALSE, 0, circleVertices);
	glVertexAttribPointer(colorsAttr, 3, GL_FLOAT, GL_FALSE, 0, circleColors);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void GLWidget::shaderLoadError()
{
	Console::redBold<<"GLWidget: error loading shaders!";
}

void GLWidget::resizeGL(int width, int height)
{
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width * retinaScale, height * retinaScale);

	qreal aspect = qreal(width) / qreal(height ? height : 1);

	const qreal zNear = 0.1, zFar = 100.0, fov = 60.0;

	perspectiveMatrix.setToIdentity();
	perspectiveMatrix.perspective(fov, aspect, zNear, zFar);
}

void GLWidget::paintGL()
{
	if(frameBuffer == NULL)
		return;
	
	glClear(GL_COLOR_BUFFER_BIT);

	QMatrix4x4 worldMatrix = perspectiveMatrix;
	worldMatrix.translate(0, 0, -5);

	program.setUniformValue(worldMatrixUniform, worldMatrix);
	
	Sphere s;
	
	while(frameBuffer->hasElements())
	{
		s = frameBuffer->popElement();
		
		QMatrix4x4 sphereMatrix;
		sphereMatrix.translate(s.pos(0), s.pos(1), s.pos(2));
		sphereMatrix.scale(s.radius);
		program.setUniformValue(sphereMatrixUniform, sphereMatrix);
		
		glDrawArrays(GL_TRIANGLE_FAN, 0, circleEdges+2);
	}
	
	frameBuffer->popFrame();
}

void GLWidget::paintBackground()
{
	// TODO
}

void GLWidget::timerUpdate()
{
	if(animating)
		animationTimer->start(sleepTime);
	updateGL();
}

void GLWidget::updateTimerFrequency(int frameBufferPercentageLevel)
{
	frameBufferPercentageLevelSum += frameBufferPercentageLevel;
	frameBufferPercentageLevelCounter++;
	if(controlTimer->elapsed()>500)
	{
		controlTimer->restart();
		float frameBufferPercentageLevelAverage = frameBufferPercentageLevelSum*1.f/frameBufferPercentageLevelCounter;
		float factor = (frameBufferPercentageLevelAverage-50)/50.f;
		factor = pow(2*factor, 3);
		float amplitude = 4;
		float fps = 60+(factor*amplitude);
		sleepTime = (quint16)std::max(0, (int)round(1000.f/fps));
		qDebug()<<"GLWidget: level:"<<frameBufferPercentageLevelAverage<<"\tfps:"<<fps<<"\tms:"<<sleepTime;
		frameBufferPercentageLevelSum = 0;
		frameBufferPercentageLevelCounter = 0;
	}
}

void GLWidget::startAnimation()
{
	if(!animating)
	{
		animating = true;
		animationTimer->start(sleepTime);
		controlTimer->start();
	}
}

void GLWidget::stopAnimation()
{
	animating = false;
}
