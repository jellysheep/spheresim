#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "CirclesHolder.h"
#include "ClTimer.h"

#include <CL/cl.hpp>

class GLWidget : public QGLWidget, public FramesCounter {

	Q_OBJECT // must include this if you use Qt signals/slots	  

protected:
	void initializeGL();
	void reboxSizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	cl_double3 rotation;
	double translateZ;
	ClTimer* clTimer;
    
public:
	GLWidget(ClTimer* ct, QWidget *parent = NULL);

public slots:
	void updateTimer();
};

#endif  /* _GLWIDGET_H */
