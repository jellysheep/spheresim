/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "GLWidget.hpp"
#include "Console.hpp"

#include <QDebug>
#include <QtOpenGL>
#include <QtGui>
#include <QTimer>
#include <QElapsedTimer>

using namespace SphereSim;

GLWidget::GLWidget(QWidget* parent)
    :QGLWidget(parent), frameBuffer(nullptr), program(this),
    verticesAttr(0), colorsAttr(0), worldMatrixUniform(0), sphereMatrixUniform(0),
    perspectiveMatrix(), animationTimer(new QTimer()),
    controlTimer(new QElapsedTimer()), sleepTime(20),
    frameBufferPercentageLevelSum(0), frameBufferPercentageLevelCounter(0),
    animating(false), circleEdges(50),
    circleVertices(nullptr), circleColors(nullptr), boxLength(1)
{
    connect(animationTimer, SIGNAL(timeout()), SLOT(timerUpdate()));
    animationTimer->setTimerType(Qt::PreciseTimer);
    animationTimer->setSingleShot(true);
}

void GLWidget::setFrameBuffer(FrameBuffer<Sphere>* fb)
{
    frameBuffer = fb;
}

void GLWidget::setBoxLength(Scalar length)
{
    boxLength = length;
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
    glEnable(GL_DEPTH_TEST);

    if (program.addShaderFromSourceFile(QGLShader::Vertex,
        ":/VertexShader.glsl") == false)
    {
        shaderLoadError();
    }
    if (program.addShaderFromSourceFile(QGLShader::Fragment,
        ":/FragmentShader.glsl") == false)
    {
        shaderLoadError();
    }
    if (program.link() == false)
    {
        shaderLoadError();
    }
    if (program.bind() == false)
    {
        shaderLoadError();
    }

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
    for (quint16 i = 1; i<circleEdges+2; i++)
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
    glViewport(0, 0, width, height);

    qreal aspect = qreal(width) / qreal(height ? height : 1);

    const qreal zNear = 0.1, zFar = 100.0, fov = 60.0;

    perspectiveMatrix.setToIdentity();
    perspectiveMatrix.perspective(fov, aspect, zNear, zFar);
}

void GLWidget::paintGL()
{
    if (frameBuffer == nullptr)
    {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 worldMatrix = perspectiveMatrix;
    worldMatrix.translate(0, 0, -1.5);
    worldMatrix.translate(-0.5, -0.5, -0.5);
    worldMatrix.scale(1/boxLength);

    program.setUniformValue(worldMatrixUniform, worldMatrix);

    QMatrix4x4 boxMatrix;
    boxMatrix.scale(boxLength);
    program.setUniformValue(sphereMatrixUniform, boxMatrix);

    float boxVertices[] =
    {
        0, 0, 0, 0, 0, 1,
        0, 0, 1, 0, 1, 1,
        0, 1, 1, 0, 1, 0,
        0, 1, 0, 0, 0, 0,

        1, 0, 0, 1, 0, 1,
        1, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 0,
        1, 1, 0, 1, 0, 0,

        0, 0, 0, 1, 0, 0,
        0, 0, 1, 1, 0, 1,
        0, 1, 1, 1, 1, 1,
        0, 1, 0, 1, 1, 0
    };

    float col = 0.5;
    float colR = col, colG = col, colB = col;
    float boxColors[] =
    {
        colR, colG, colB, colR, colG, colB, colR, colG, colB, colR, colG, colB,
        colR, colG, colB, colR, colG, colB, colR, colG, colB, colR, colG, colB,
        colR, colG, colB, colR, colG, colB, colR, colG, colB, colR, colG, colB,
        colR, colG, colB, colR, colG, colB, colR, colG, colB, colR, colG, colB,
        colR, colG, colB, colR, colG, colB, colR, colG, colB, colR, colG, colB,
        colR, colG, colB, colR, colG, colB, colR, colG, colB, colR, colG, colB
    };

    glVertexAttribPointer(verticesAttr, 3, GL_FLOAT, GL_FALSE, 0, boxVertices);
    glVertexAttribPointer(colorsAttr, 3, GL_FLOAT, GL_FALSE, 0, boxColors);

    glDrawArrays(GL_LINES, 0, 24);

    glVertexAttribPointer(verticesAttr, 2, GL_FLOAT, GL_FALSE, 0, circleVertices);
    glVertexAttribPointer(colorsAttr, 3, GL_FLOAT, GL_FALSE, 0, circleColors);

    Sphere s;

    while (frameBuffer->hasElements())
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
    if (animating)
    {
        animationTimer->start(sleepTime);
    }
    updateGL();
}

void GLWidget::updateTimerFrequency(int frameBufferPercentageLevel)
{
    frameBufferPercentageLevelSum += frameBufferPercentageLevel;
    frameBufferPercentageLevelCounter++;
    if (controlTimer->elapsed()>500)
    {
        controlTimer->restart();
        Scalar frameBufferPercentageLevelAverage =
            frameBufferPercentageLevelSum*1.f/frameBufferPercentageLevelCounter;
        Scalar factor = (frameBufferPercentageLevelAverage-50)/50.0;
        factor = pow(factor, 5);
        Scalar amplitude = 20;
        Scalar fps = 60+(factor*amplitude);
        sleepTime = (quint16)(std::max)(0, (int)std::round(1000.0/fps));
        qDebug()<<"GLWidget: level:"<<frameBufferPercentageLevelAverage
            <<"\tfps:"<<fps<<"\tms:"<<sleepTime;
        frameBufferPercentageLevelSum = 0;
        frameBufferPercentageLevelCounter = 0;
    }
}

void GLWidget::startAnimation()
{
    if (animating == false)
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
