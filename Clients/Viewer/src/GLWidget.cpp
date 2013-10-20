/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <GLWidget.hpp>

#include <QDebug>
#include <QtOpenGL>
#include <QtGui>

using namespace SphereSim;

GLWidget::GLWidget(QWidget* parent):QGLWidget(parent), program(this)
{
	frames = 0;
	frameBuffer = NULL;
}

void GLWidget::setFrameBuffer(FrameBuffer<Sphere>* fb)
{
	frameBuffer = fb;
}

GLWidget::~GLWidget()
{
	program.release();
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
	
	posAttr = program.attributeLocation("posAttr");
	colAttr = program.attributeLocation("colAttr");
	matrixUniform = program.uniformLocation("matrix");
}

void GLWidget::initShaders()
{
}

void GLWidget::shaderLoadError()
{
	qDebug()<<"GLWidget: error loading shaders!";
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

	program.bind();

	QMatrix4x4 matrix = perspectiveMatrix;
	matrix.translate(0, 0, -3);
	//matrix.rotate(100.0f * frames / 60, 0, 1, 0);

	program.setUniformValue(matrixUniform, matrix);
	
	Sphere s;
	
	while(frameBuffer->hasElements())
	{
		s = frameBuffer->popElement();
		float x = s.pos(0);
		float y = s.pos(1);
		
		GLfloat vertices[] = {
			x+0.0f,	y+0.866f-0.5f,
			x-0.5f, y-0.5f,
			x+0.5f, y-0.5f
		};

		GLfloat colors[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
		glVertexAttribPointer(colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}
	frameBuffer->popFrame();

	program.release();
	
	frames++;
}

void GLWidget::paintBackground()
{
	// TODO
}
