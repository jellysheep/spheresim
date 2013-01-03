#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "CirclesHolder.h"
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
	cl_double3 rotation;
	double translateZ;
	ClTimer* clTimer;
     QPoint lastPos;
    
public:
	GLWidget(ClTimer* ct, QWidget *parent = NULL);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	bool drawingFinished;

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
