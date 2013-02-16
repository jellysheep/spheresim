
#include "PlotWidget.h"
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

PlotWidget::PlotWidget(const char* str, QWidget* parent):QDockWidget(str,parent),xData(numSamples),yData(numSamples){
	plot = new QwtPlot();
	//plot->setTitle("Wall forces");
	plot->setCanvasBackground(Qt::white);
	//plot->setAxisScale(QwtPlot::yLeft, 0.0, 100.0);
	QwtLogScaleEngine* engine = new QwtLogScaleEngine();
	//double a = 10.0, b = 0.1, c = 100.0, d = 10.0;
	//engine->divideScale(0.1,100,3,3);
	//engine->autoScale(a,b,c,d);
	plot->setAxisScaleEngine(QwtPlot::yLeft, engine);
	//plot->setAxisScaleDiv(QwtPlot::yLeft, engine->divideScale(0.1,100,100,100,0));
	plot->setAxisScale(QwtPlot::yLeft, 0.1, 200.0);
	plot->insertLegend(new QwtLegend());
	
	QSize size = plot->minimumSizeHint();
	size.setHeight(200);
	//plot->setMinimumSize(size);
	//plot->resize(size);
	plot->setFixedHeight(200);
	plot->setMinimumWidth(200);
	
	grid = new QwtPlotGrid();
	grid->attach(plot);
	
	curve = new QwtPlotCurve();
    curve->setTitle("Wall 1");
    curve->setPen(Qt::blue, 1.5);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    fitter = new QwtSplineCurveFitter();
    //fitter->setSplineSize(20);
    curve->setCurveFitter(fitter);
    curve->setCurveAttribute(QwtPlotCurve::Fitted, true);
    
    symbol = new QwtSymbol(QwtSymbol::Ellipse, 
		QBrush(Qt::red), QPen(), QSize(1.5, 1.5));
		//QBrush(Qt::yellow), QPen(Qt::red, 2), QSize(8, 8));
    curve->setSymbol(symbol);
    
    for(int i = 0; i<numSamples; i++){
		xData[i] = i;
		yData[i] = 0;
	}
    curve->setSamples(xData,yData);

    curve->attach(plot);
	
	setWidget(plot);
}

void PlotWidget::addValue(double v){
	yData.pop_front();
	yData.append(v);
    curve->setSamples(xData,yData);
	plot->replot();
}
