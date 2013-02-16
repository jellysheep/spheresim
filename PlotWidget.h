
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
	QwtPlotCurve *curve;
	QwtSymbol *symbol;
    QwtSplineCurveFitter* fitter;
    QVector<double> xData, yData;
public:
	PlotWidget(const char* str, QWidget* parent=NULL);
public slots:
	void addValue(double v);
};
