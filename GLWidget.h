#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QKeyEvent>
#include "FramesCounter.h"
#include "Calculator.h"
#include "Circles.h"

#include <CL/cl.hpp>

class Calculator;

struct ViewOptions {
	scalar xRot, yRot, zRot;
	scalar xRotCam, yRotCam;
	scalar transX, transY, transZ;
};

class GLWidget : public QGLWidget, public FramesCounter {

	Q_OBJECT // must include this if you use Qt signals/slots	 

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	ViewOptions curView;
	vector3 rotation;
	scalar translate;
	Calculator* clTimer;
    QPoint lastPos;
    
    scalar rotGrav;
    
    GLfloat** LightPosition;
    
    void normalize(GLfloat *a);
    void drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r);
	
	void reflect();
	void drawQuad(int i);
	
	void drawBoxSides();
	
	const static ViewOptions initView;
	ViewOptions lastView, deltaView;
	bool resettingView;
	int resetCounter, resetCount;
	QTimer *resetTimer;
	void resetView(int ms);
	scalar resetFact;
    
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
	void resetView();
	void resetViewTimer();

signals:
	void timeToRender_();
};

#endif  /* _GLWIDGET_H */
