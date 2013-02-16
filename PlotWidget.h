
#include <QObject>
#include <QtGui/QDockWidget>

class QwtPlot;

class PlotWidget : public QDockWidget {
	Q_OBJECT
protected:
	QwtPlot *plot;
	QwtPlotGrid *grid;
public:
	PlotWidget(const char* str, QWidget* parent=NULL);
};
