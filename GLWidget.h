#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QKeyEvent>
#include "FramesCounter.h"
#include "Calculator.h"
#include "Circles.h"

#include <CL/cl.hpp>

class Calculator;

class GLWidget : public QGLWidget, public FramesCounter {

	Q_OBJECT // must include this if you use Qt signals/slots	 

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	scalar xRot,yRot,zRot;
	scalar xRotCam,yRotCam;
	vector3 rotation;
	scalar translate;
	scalar transX, transY, transZ;
	Calculator* clTimer;
    QPoint lastPos;
    
    scalar rotGrav;
    
    GLfloat** LightPosition;
    
    void normalize(GLfloat *a);
    void drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r);
	
	void reflect();
	void drawQuad(int i);
	
	void drawBoxSides();
    
public:
	GLWidget(Calculator* ct, QWidget *parent = NULL);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	bool drawingFinished;
    QTimer* rotationTimer;
    bool newFrame;
    
    void drawsphere(int ndiv, float radius);
    void drawsphere2(float radius);
    void drawCircleF(float radius);
    void drawCircleF2(float radius);
	
	unsigned int displayList;
	
	void drawLights();
	
	void setLightPos();

public slots:
	void setXRotation(scalar angle);
	void setYRotation(scalar angle);
	//void setZTranslate(int angle);
	void timeToRender();
	void timeToRender2();
	void setRenderFps(double fps);
	void updateTimer();

signals:
	void timeToRender_();
};

#endif  /* _GLWIDGET_H */
