#include <stdlib.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include "data_plot.h"

TDataPlot::TDataPlot(int nGr,QWidget *parent,QString title): QwtPlot(parent),numGraph(nGr)
{
  // Disable polygon clipping
  //QwtPainter::setDeviceClipping(false);

  // We don't need the cache here
 // canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
 // canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);
  canvas()->installEventFilter(this);
  grid=new QwtPlotGrid;
  grid->attach(this);
  grid->hide();
  grid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));

  alignScales();
  setCanvasBackground(QBrush(qRgb(255,255,255)));

    // Assign a title
    setTitle(title);
  //  insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

  for(int i=0;i<numGraph;++i){
    massGr.append(new QwtPlotCurve());
    massGr[i]->attach(this);

  }

  QwtText titleX("time, ms");
  titleX.setFont(QFont("Tahoma",14));
  titleX.setColor(qRgb(51,51,51));
  setAxisFont(QwtPlot::xBottom,QFont("Tahoma",12));
  setAxisTitle(QwtPlot::xBottom, titleX);
  setAxisScale(QwtPlot::xBottom, 1, 5);
  setAxisMaxMajor(QwtPlot::xBottom, 5);
  setAxisMaxMinor(QwtPlot::xBottom, 1);

  setAxisFont(QwtPlot::yLeft,QFont("Tahoma",12));
  QwtText titleY("U, kV");
  titleY.setFont(QFont("Tahoma",14));
  titleY.setColor(qRgb(51,51,51));
  setAxisTitle(QwtPlot::yLeft, titleY);
}
// set grid
void TDataPlot::setGridVisible(bool visible)
{
  if(visible)
    grid->show();
  else
    grid->hide();
}
//
//  Set a plain canvas frame and align the scales to it
//
void TDataPlot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

   // canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
   // canvas()->setLineWidth(1);
    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}
// axis setup
void TDataPlot::setAxis(void)
{
  // find max Y value
  double maxY=0,minY=10000;
  double ySc;
  for(int i=0;i<massGr.size();i++){
    //xSc=massGr[i]->minYValue();
    if(maxY<massGr[i]->maxYValue())
      if(massGr[i]->isVisible()) maxY=massGr[i]->maxYValue();
    if(minY>massGr[i]->minYValue())
      if(massGr[i]->isVisible()) minY=massGr[i]->minYValue();
  }

  // Axis setup

  double majorY;

  ySc=fabs(minY+maxY);
  if(ySc<1)majorY=0.1;
  else if(ySc<5)majorY=0.5;
  else if(ySc<10)majorY=1;
  else if(ySc<=50) majorY=5;
  else if(ySc<=100) majorY=10;
  else if(ySc<=500) majorY=50;
  else if(ySc<=1000) majorY=100;
  else if(ySc<=2000) majorY=200;
  else if(ySc<=5000) majorY=500;
  else if(ySc<=10000) majorY=1000;
  else if(ySc<=20000) majorY=2000;
  else majorY=5000;
 // setAxisScale(QwtPlot::xBottom, 1, (dim/5+1)*5,major);
 // qDebug()<<xSc<<(massGr[0]->maxXValue())<<massGr.size();
  //qDebug()<<xSc<<major;
  int minorY=round(massGr[0]->maxXValue()/200)*200;
  setAxisScale(QwtPlot::xBottom, 0, massGr[0]->maxXValue(),5);
  //setAxisMaxMinor(QwtPlot::xBottom, 1);
  setAxisScale(QwtPlot::yLeft, minY, maxY);
 // qDebug()<<"mmax "<<minY<<maxY<<majorY;
}
// setup data for graph
void TDataPlot::setDataForGraph(int numGr,int xySize,QRgb color,const double *dataX, const double *dataY)
{
  dim=xySize;
  //qDebug()<<"Set gr"<<dim<<numGr<<massGr.size();
  massGr[numGr]->setRawSamples(dataX,dataY,dim);
  massGr[numGr]->setPen(QColor(color));

}
// set visible graph and rercalc axis
void TDataPlot::setVisibleMap(const bool map[])
{
  for(int i=0;i<massGr.size();i++){
    massGr[i]->setVisible(map[i]);
  }
  setAxis();
}
void TDataPlot::replotGraph(void)
{
  if(dim>=1) replot();
}
