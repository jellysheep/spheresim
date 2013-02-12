#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
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
	int xRot,yRot,zRot;
	vector3 rotation;
	scalar translateZ;
	Calculator* clTimer;
    QPoint lastPos;
    
    scalar rotGrav;
    
    GLfloat** LightPosition;
    
    void normalize(GLfloat *a);
    void drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r);
	void timeToRender2();
    
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

public slots:
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZTranslate(int angle);
	void timeToRender();
	void setRenderFps(double fps);

signals:
	void timeToRender_();

public slots:
	void updateTimer();
};

#endif  /* _GLWIDGET_H */
