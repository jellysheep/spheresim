/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _GLWIDGET_HPP_
#define _GLWIDGET_HPP_

#include <FrameBuffer.hpp>

#include <QGLWidget>
#include <QGLFunctions>
#include <QMatrix4x4>
#include <QGLShaderProgram>

class QTimer;
class QElapsedTimer;

namespace SphereSim
{
    /** \brief Custom OpenGL widget of the main window. */
    class GLWidget : public QGLWidget, protected QGLFunctions
    {
        Q_OBJECT

    private:
        /** \brief FrameBuffer for smooth animation. */
        FrameBuffer<Sphere>* frameBuffer;

        /** \brief GLSL program used to render the spheres. */
        QGLShaderProgram program;

        /** \brief Vertex attribute for circle vertices. */
        GLuint verticesAttr;

        /** \brief Vertex attribute for circle colors. */
        GLuint colorsAttr;

        /** \brief Matrix uniform for world transformations. */
        GLuint worldMatrixUniform;

        /** \brief Matrix uniform for sphere transformations. */
        GLuint sphereMatrixUniform;

        /** \brief Matrix to set the scene perspective. */
        QMatrix4x4 perspectiveMatrix;

        /** \brief Timer used to animate the scene. */
        QTimer* animationTimer;

        /** \brief Timer used to periodically update framerate. */
        QElapsedTimer* controlTimer;

        /** \brief Delay (in milliseconds) used for the animation timer. */
        quint16 sleepTime;

        /** \brief Sum of last FrameBuffer percentage levels. */
        int frameBufferPercentageLevelSum;

        /** \brief Number of last FrameBuffer percentage levels. */
        int frameBufferPercentageLevelCounter;

        /** \brief Flag for running animation. */
        bool animating;

        /** \brief Number of edges used to render a circle. */
        const quint16 circleEdges;

        /** \brief Vertices used to render a circle. */
        float* circleVertices;

        /** \brief Colors used to render a circle. */
        float* circleColors;

        Scalar boxLength;

        /** \brief Initialize the OpenGL scene. */
        void initializeGL();

        void initShaders();

        /** \brief Resize the OpenGL widget. */
        void resizeGL(int width, int height);

        /** \brief Paint the OpenGL scene. */
        void paintGL();

        /** \brief Paint the background. */
        void paintBackground();

        /** \brief Error handler for shader loading. */
        void shaderLoadError();

    public:
        /** \brief Initialize member variables. */
        GLWidget(QWidget* parent);

        /** \brief Clean up member variables. */
        ~GLWidget();

        GLWidget() = delete;
        GLWidget(const GLWidget&) = delete;
        GLWidget& operator=(const GLWidget&) = delete;

        void setFrameBuffer(FrameBuffer<Sphere>* frameBuffer);

        void setBoxLength(Scalar boxLength);

    public slots:
        /** \brief Animation timer update. */
        void timerUpdate();

        /** \brief Update the animation framerate. */
        void updateTimerFrequency(int frameBufferPercentageLevel);

        /** \brief Start the animation. */
        void startAnimation();

        /** \brief Stop the animation. */
        void stopAnimation();
    };
}

/** \brief Make GLWidget known to Qt GUI classes. */
typedef SphereSim::GLWidget GLWidget;

#endif
