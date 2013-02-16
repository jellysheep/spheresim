
#include "PlotWidget.h"
#include <qwt/qwt_plot.h>

PlotWidget::PlotWidget(const char* str, QWidget* parent):QDockWidget(str,parent){
	plot = new QwtPlot();
	plot->setTitle("Wall forces");
	plot->setCanvasBackground(Qt::white);
	plot->setAxisScale(QwtPlot::yLeft, 0.0, 10.0);
	plot->insertLegend(new QwtLegend());
	grid = new QwtPlotGrid();
	grid->attach(plot);
	
	setWidget(plot);
}
