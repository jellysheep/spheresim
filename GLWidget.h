#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "FramesCounter.h"
#include "ClTimer.h"

#include <CL/cl.hpp>

class ClTimer;

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
	ClTimer* clTimer;
    QPoint lastPos;
    
    GLfloat* LightPosition;
    
    void normalize(GLfloat *a);
    void drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r);
	void timeToRender2();
    
public:
	GLWidget(ClTimer* ct, QWidget *parent = NULL);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	bool drawingFinished;
    QTimer* rotationTimer;
    bool newFrame;
    
    void drawsphere(int ndiv, float radius);

public slots:
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);
	void timeToRender();

signals:
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);
	void timeToRender_();

public slots:
	void updateTimer();
};

#endif  /* _GLWIDGET_H */
