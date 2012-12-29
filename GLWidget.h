#ifndef _GLWIDGET_H
#define _GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "CirclesHolder.h"

class GLWidget : public QGLWidget {

	Q_OBJECT // must include this if you use Qt signals/slots      

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	double rotation;
	CirclesHolder* circlesHolder;

public:
	GLWidget(CirclesHolder* ch, QWidget *parent = NULL);

public slots:
	void updateTimer();
};

#endif  /* _GLWIDGET_H */
