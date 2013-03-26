
#include "PlotWidget.h"
#include "Spheres.h"
#include <QPolygon>
#include <QPoint>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_curve_fitter.h>
#include <qwt/qwt_scale_engine.h>
#include <qwt/qwt_transform.h>

const int PlotWidget::numSamples = 100;

PlotWidget::PlotWidget(const char* str, QWidget* parent):QDockWidget(str,parent),xData1(0),yData1(0),xData2(0),yData2(0){
	plot = new QwtPlot();
	//plot->setTitle("Wall forces");
	plot->setCanvasBackground(Qt::white);
	plot->setAxisScale(QwtPlot::xBottom, -numSamples*1.0/plotFps, 0.0);
	//plot->setAxisScale(QwtPlot::yLeft, 0.0, 100.0);
	QwtLogScaleEngine* engine = new QwtLogScaleEngine();
	//double a = 10.0, b = 0.1, c = 100.0, d = 10.0;
	//engine->divideScale(0.1,100,3,3);
	//engine->autoScale(a,b,c,d);
	plot->setAxisScaleEngine(QwtPlot::yLeft, engine);
	//plot->setAxisScaleDiv(QwtPlot::yLeft, engine->divideScale(0.1,100,100,100,0));
	plot->setAxisScale(QwtPlot::yLeft, 0.1, 300.0);
	plot->insertLegend(new QwtLegend());
	
	QSize size = plot->minimumSizeHint();
	size.setHeight(200);
	//plot->setMinimumSize(size);
	//plot->resize(size);
	plot->setFixedHeight(200);
	plot->setMinimumWidth(200);
	
	grid = new QwtPlotGrid();
	grid->attach(plot);
	
	curve1 = new QwtPlotCurve();
	curve1->setTitle("Wall 1");
	curve1->setPen(Qt::blue, 1.5);
	curve1->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	fitter = new QwtSplineCurveFitter();
	//fitter->setSplineSize(20);
	curve1->setCurveFitter(fitter);
	curve1->setCurveAttribute(QwtPlotCurve::Fitted, true);
	
	symbol1 = new QwtSymbol(QwtSymbol::Ellipse, 
		QBrush(Qt::blue), QPen(Qt::blue, 2), QSize(1.5, 1.5));
	curve1->setSymbol(symbol1);
	curve1->setSamples(xData1,yData1);
	curve1->attach(plot);
	
	curve2 = new QwtPlotCurve();
	curve2->setTitle("Wall 2");
	curve2->setPen(Qt::red, 1.5);
	curve2->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	fitter = new QwtSplineCurveFitter();
	//fitter->setSplineSize(20);
	curve2->setCurveFitter(fitter);
	curve2->setCurveAttribute(QwtPlotCurve::Fitted, true);
	
	symbol2 = new QwtSymbol(QwtSymbol::Ellipse, 
		QBrush(Qt::red), QPen(Qt::red, 2), QSize(1.5, 1.5));
	curve2->setSymbol(symbol2);
	curve2->setSamples(xData2,yData2);
	curve2->attach(plot);
	
	setWidget(plot);
}

void PlotWidget::addValue(double v, double w){
	if(v<=0) return;
	if(w<=0) return;
	v = std::max(0.1,v);
	w = std::max(0.1,w);
	//printf("value: %5f\n", v);
	
	if(xData1.size()<numSamples){
		xData1.prepend((-xData1.size())*1.0/plotFps);
	}
	if(yData1.size()>=numSamples){
		yData1.pop_front();
	}
	yData1.append(v);
	
	if(xData2.size()<numSamples){
		xData2.prepend((-xData2.size())*1.0/plotFps);
	}
	if(yData2.size()>=numSamples){
		yData2.pop_front();
	}
	yData2.append(w);
	curve1->setSamples(xData1,yData1);
	curve2->setSamples(xData2,yData2);
	plot->replot();
}
