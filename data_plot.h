#ifndef _DATA_PLOT_H
#define _DATA_PLOT_H
#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>

const uint32_t colorGraph[16]={0xff000000,0xff87CEEB,0xff0000FF,0xffFF00FF,
                               0xffFF91A4,0xff808080,0xff008000,0xff66CDAA,
                               0xff800000,0xff000080,0xff808000,0xff800080,
                               0xffFF0000,0xffC8A2C8,0xff008080,0xffF95A61 };

class  TDataPlot : public QwtPlot
{
    Q_OBJECT
public:
    TDataPlot(int nGr=1,QWidget* = NULL,QString title="Current");
    QVector <QwtPlotCurve*> massGr;
    QwtPlotGrid *grid;
    void setGridVisible(bool);
    void setDataForGraph(int numGr,int xySize,QRgb color,const double *dataX, const double *dataY);
    void setAxis(void);
    void setVisibleMap(const bool map[]);
    void replotGraph(void);
    QwtPlotCurve *crv;
private:
    void alignScales();
    int numGraph;
    int dim;
};

#endif
