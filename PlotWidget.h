
#include <QObject>
#include <QtGui/QDockWidget>

class QwtPlot;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtSymbol;
class QPolygonF;
class QwtCurveFitter;
class QwtSplineCurveFitter;

class PlotWidget : public QDockWidget {
	Q_OBJECT
protected:
	const static int numSamples;
	QwtPlot *plot;
	QwtPlotGrid *grid;
	QwtPlotCurve *curve1, *curve2;
	QwtSymbol *symbol1, *symbol2;
	QwtSplineCurveFitter* fitter;
	QVector<double> xData1, yData1, xData2, yData2;
public:
	PlotWidget(const char* str, QWidget* parent=NULL);
public slots:
	void addValue(double v, double w);
};
