#include "main_refrf.h"
//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------
Main_TD::Main_TD(QWidget *parent)  : QMainWindow(parent)
{
  dataFN="";
  dataFNCSV="";
  http=new THttpProc;

  slot_readSettings();
  sortData();
  create_ListWidget();
  create_Menu();
  create_StatusBar();

  QApplication::processEvents();

  //... Text Codec ...
  QTextCodec *russianCodec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForLocale(russianCodec);

  //... Data&Time ...
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(slot_updateDateTime()));
  //slot_updateDateTime();
  timer->start(2000);
  alarmWrTimer = new QTimer(this);
  connect(alarmWrTimer, SIGNAL(timeout()), this, SLOT(slot_alarmWriteAnswer()));
  setWindowTitle(tr("Program for setup RF amplitude"));
  //slot_writeSettings();
  http->setDeviceAddress(httpAddress);
  modifyData=false;
  slot_plotGraph();
  setMinimumSize(800,600);
  showMaximized();
 // resize(800,600);
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
Main_TD::~Main_TD()
{    
  for(int i=0;i<32;i++) {
    delete itemTable[0][i];
    delete itemTable[1][i];
  }
  http->terminate();
  delete http;
}
void Main_TD::keyPressEvent(QKeyEvent *event)
{
   qDebug()<<"keyPressed"<<event->key();
   modifyData=true;
   QWidget::keyPressEvent(event);
}
//-----------------------------------------------------------------------------
//--- create ListWidget
//-----------------------------------------------------------------------------
void Main_TD::create_ListWidget()
{
  MainGroupBox = new QGroupBox(tr("Prepare data"),this);
  MainGroupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
  setCentralWidget(MainGroupBox);
  main_layout = new QVBoxLayout();
  main_layout->setMargin(5);
  main_layout->setSpacing(4);

  refresh_btn=new QPushButton(tr("Refresh"),this);
  read_btn=new QPushButton(tr("Load"),this);
  save_btn=new QPushButton(tr("Save"),this);
  savecsv_btn=new QPushButton(tr("Execute"),this);
  mult_btn=new QPushButton(tr("Multiply"),this);
  undo_btn=new QPushButton(tr("< Undo"),this);
  undo_btn->setEnabled(false);
 // time1_Label=new QLabel(tr("Time step, ms"),this);
 // transf_Label=new QLabel(tr("Coefficient of transf Vrf/Vgf"),this);

  //coefTransf_Edit=new QSpinBox(this); coefTransf_Edit->setMinimum(1); coefTransf_Edit->setMaximum(10000);

  multCoeffLabel=new QLabel(tr("Multiply coefficien for RF amplitude"));
  multCoeffSB=new QDoubleSpinBox();
  multCoeffSB->setSingleStep(0.01);
  multCoeffSB->setMaximum(2);
  multCoeffSB->setMinimum(0.1);
  multCoeffSB->setDecimals(2);
  multCoeffSB->setValue(1);

  edit_layout = new QHBoxLayout;
  edit_layout->addWidget(read_btn); edit_layout->addStretch(1);
  edit_layout->addWidget(save_btn); edit_layout->addStretch(1);
  edit_layout->addWidget(refresh_btn); edit_layout->addStretch(1);
  edit_layout->addWidget(undo_btn); edit_layout->addStretch(1);
  edit_layout->addWidget(savecsv_btn); edit_layout->addStretch(1);
  edit_layout->addWidget(multCoeffLabel); //edit_layout->addStretch(1);
  edit_layout->addWidget(multCoeffSB); edit_layout->addStretch(1);
  edit_layout->addWidget(mult_btn); edit_layout->addStretch(1);

  //edit_layout->addWidget(transf_Label);edit_layout->addWidget(coefTransf_Edit); edit_layout->addStretch(1);
  edit_layout->setSpacing(10);

  data_layout=new QHBoxLayout;

  tableRf=new QTableWidget(this);
  //tableRf->verticalHeader()->hide();
  tableRf->setRowCount(2);
  tableRf->setColumnCount(32);
  //QTableWidgetItem *labelItem1= new QTableWidgetItem(tr("time, us"));labelItem1->setBackgroundColor(HEADER_COLOR); tableRf->setItem(0,0,labelItem1);
  QStringList headerV;
  //headerH<<""<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11";
  headerV<<tr("time, ms")<<tr("RF, kV");
  tableRf->setVerticalHeaderLabels(headerV);
  tableRf->horizontalHeader()->setStretchLastSection(false);
  tableRf->verticalHeader()->setStretchLastSection(false);
  tableRf->setMinimumHeight(120);
  tableRf->setMaximumHeight(130);
  //tableRf->setColumnWidth(0,20);
  //tableRf->resizeColumnsToContents();
  data_layout->addWidget(tableRf);
  QLabel graphLabel;
  plotRF=new TDataPlot(2,&graphLabel,tr("Voltage in cavity"));
  graph_layout=new QHBoxLayout;
  graph_layout->addWidget(plotRF);
  plotRF->setGridVisible(true);

  main_layout->addLayout(data_layout);
  main_layout->addLayout(graph_layout);
  main_layout->addLayout(edit_layout);

  MainGroupBox->setLayout(main_layout);

// create table items
 for(int i=0;i<ALLVECTORS;i++) {
   itemTable[0][i]=new QTableWidgetItem();
   tableRf->setItem(0,i,itemTable[0][i]);
   itemTable[1][i]=new QTableWidgetItem();
   tableRf->setItem(1,i,itemTable[1][i]);
   tableRf->setColumnWidth(i,45);
 }
 putDataToTable();
 connect(refresh_btn,SIGNAL(pressed()), this, SLOT(slot_plotGraph()));
 connect(save_btn,SIGNAL(pressed()), this, SLOT(slot_saveDataFile()));
 connect(read_btn,SIGNAL(pressed()), this, SLOT(slot_readDataFile()));
 connect(savecsv_btn,SIGNAL(pressed()), this, SLOT(slot_saveCSVDataFile()));
 connect(mult_btn,SIGNAL(pressed()), this, SLOT(slot_multData()));
 connect(undo_btn,SIGNAL(pressed()), this, SLOT(slot_undoData()));
}

void Main_TD::putDataToTable(void)
{
  for(int i=0;i<ALLVECTORS;i++) {
    itemTable[0][i]->setText(QString("%1").arg(data[0][i],4,'f',1));
    itemTable[1][i]->setText(QString("%1").arg(data[1][i],5,'f',2));
  }
}

void Main_TD::getDataFromTable(void)
{
  bool ok;
  double tmp;
  for(int i=0;i<ALLVECTORS;i++) {
    tmp=itemTable[0][i]->text().toDouble(&ok);
    if(tmp<0) tmp=0;
    else if(tmp>maxTime)
      tmp=maxTime;
    tmp=round(tmp*10);
    data[0][i]=tmp/10.0;
    if(!ok) data[0][i]=0;

    tmp=itemTable[1][i]->text().toDouble(&ok);
    if(tmp<0) tmp=0;
    else if(tmp>maxAmp)
      tmp=maxAmp;
    tmp=round(tmp*100);
    data[1][i]=tmp/100.0;
    if(!ok) data[1][i]=0;
  }
}

void Main_TD::sortData()
{
  double v1;
  for(int i=0;i<ALLVECTORS;i++){
    if(data[0][i]>maxTime){
      data[0][i]=maxTime;
    }
    if(data[0][i]<0){
      data[0][i]=0;
    }
  }
  for(int i=0;i<ALLVECTORS;i++){
    if(data[1][i]>maxAmp){
      data[1][i]=maxAmp;
    }
    if(data[1][i]<0){
      data[1][i]=0;
    }
  }
// remove same data
  for(int i=0;i<ALLVECTORS-1;i++){
    v1=data[0][i];
    if(v1){
      for(int j=i+1;j<ALLVECTORS;j++){
        if(v1==data[0][j]){
          data[0][j]=0;data[1][j]=0;
        }
      }
    }
  }
//sort data
  for(int i=1;i<ALLVECTORS;i++){
    for(int j=0;j<ALLVECTORS-i;j++){
      if(data[0][j]>data[0][j+1]){
        v1=data[0][j+1];
        data[0][j+1]=data[0][j];
        data[0][j]=v1;
        v1=data[1][j+1];
        data[1][j+1]=data[1][j];
        data[1][j]=v1;
      }
      if((data[0][j]==0)&&(data[0][j+1]==0)){
        if(data[1][j]>data[1][j+1]){
          v1=data[1][j+1];
          data[1][j+1]=data[1][j];
          data[1][j]=v1;
        }
      }
    }
  }
  if(data[0][0]) data[0][0]=0;
}


//-----------------------------------------------------------------------------
//--- create Menu
//-----------------------------------------------------------------------------
void Main_TD::create_Menu()
{
  QFont font = app_font;
  font.setBold(false);
  menuBar()->setFont(font);

  file_menu = menuBar()->addMenu(tr("&File"));
  exit = new QAction(tr("Exit"), this);
  file_menu->addAction(exit);
  connect(exit,SIGNAL(triggered(bool)), this, SLOT(close()));

}

//-----------------------------------------------------------------------------
//--- create StatusBar
//-----------------------------------------------------------------------------
void Main_TD::create_StatusBar()
{
  mode_label = new QLabel(this);

  mode_label->setFixedWidth(240);
  mode_label->setAlignment(Qt::AlignCenter);

  QLabel *version_label = new QLabel(this);
  version_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  version_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  version_label->setText("ver. " + QCoreApplication::applicationVersion() + " ");

  workfile_label = new QLabel(this);
  workfile_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  workfile_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  status_Label=new QLabel(tr("Start program"),this);
  status_Label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  err_Label=new QLabel(tr("unknown"),this);
  err_Label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  statusBar()->addWidget(mode_label);
  statusBar()->addWidget(workfile_label,1);

  statusBar()->addWidget(status_Label,1);
  statusBar()->addWidget(err_Label,1);
  statusBar()->addWidget(version_label,1);

  QFont font = app_font;
  font.setBold(false);
  statusBar()->setFont(font);
}

//-----------------------------------------------------------------------------
//--- Sleep
//-----------------------------------------------------------------------------
void Main_TD::Sleep(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, SLOT(quit()));
    loop.exec();
}

//------------------------------ SLOTS ----------------------------------------
//-----------------------------------------------------------------------------
//--- Slot update_DateTime()
//-----------------------------------------------------------------------------
void Main_TD::slot_updateDateTime()
{
    timer->stop();
    QString tim = QDateTime::currentDateTime().toString(" d MMMM dddd yyyy, h:mm:ss ");
    mode_label->setText(tim);
    QFileInfo wf(dataFN);
    workfile_label->setText("Work file:  "+wf.fileName());
    QStringList sl;
    http->getStatus(sl);
    qDebug()<<"STATUS"<<sl;
    if(!sl.empty()){
      status_Label->setText(" Status: "+sl[2]);
      err_Label->setText(" Error: "+sl[1]);
      if(sl[3]=='0') {
        alarmWrTimer->stop();
        savecsv_btn->setEnabled(true);
      }
    }
    timer->start(2000);
}



//-----------------------------------------------------------------------------
//--- public slot readSettings()
//-----------------------------------------------------------------------------
void Main_TD::slot_readSettings(void)
{
  QString dir_path = qApp->applicationDirPath();
  QSettings setup(dir_path+"/setup.ini", QSettings::IniFormat);
  bool ok;
  httpAddress=setup.value("address","192.168.10.30").toString();
  stepTime=setup.value("stepTime",0.1).toDouble(&ok); if(!ok)stepTime=0.1;
  coefTransf=setup.value("coefTransf",1.2).toDouble(&ok); if(!ok)coefTransf=1.2;
  interpol=setup.value("interpol",1).toInt(&ok); if(!ok) { stepTime=0.1; interpol=1;}
  QSettings setupFn(dir_path+"/rf.ini", QSettings::IniFormat);
  if(dataFN.isEmpty()) dataFN=setupFn.value("currentFN",dir_path+"/rf.dat").toString();
  if(dataFNCSV.isEmpty()) dataFNCSV=setupFn.value("currentFNCSV",dir_path+"/rf.csv").toString();

  for(int i=0;i<ALLVECTORS;i++) { data[0][i]=0;data[1][i]=0;}
  QFile file(dataFN);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
  uint32_t i=0;
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList list=line.split(' ');
    double t,val;
    bool ok;
    if(list.size()!=2){
      t=0; val=0;
    }
    else {
      t=list.at(0).toDouble(&ok); if(!ok) t=0;
      val=list.at(1).toDouble(&ok); if(!ok) val=0;
    }
    data[0][i]=t;data[1][i]=val;
    i++;
    if(i>=ALLVECTORS) break;
  }
  file.close();
}

//-----------------------------------------------------------------------------
//--- public slot writeSettings()
//-----------------------------------------------------------------------------
void Main_TD::slot_writeSettings(void)
{
  QString dir_path = qApp->applicationDirPath();
  QSettings *setupFn=new QSettings(dir_path+"/rf.ini", QSettings::IniFormat);

  setupFn->setValue("currentFN",dataFN);
  setupFn->setValue("currentFNCSV",dataFNCSV);

  if(setupFn) delete setupFn;

  QFile file(dataFN);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&file);
  double val;
  bool ok;
  for(int i=0;i<ALLVECTORS;i++) {
    val=itemTable[0][i]->text().toDouble(&ok);
    if(ok) data[0][i]=val;
    else
      data[0][i]=0;
    val=itemTable[1][i]->text().toDouble(&ok);
    if(ok) data[1][i]=val;
    else
      data[1][i]=0;
  }
  sortData();
  for(int i=0;i<ALLVECTORS;i++) {
    out <<data[0][i]<<' '<<data[1][i]<<'\n';
  }
  file.close();
}

void Main_TD::slot_plotGraph()
{
  if(modifyData) pushData();
  getDataFromTable();
  sortData();
  putDataToTable();
  //slot_writeSettings();

  double t=0,dt=stepTime;
  int ng=0;// number of graph point
  xGr.clear();yGr.clear();
// find first none zero data
  int nPoint=0;
  while(data[0][nPoint]==0) {
    if(nPoint>ALLVECTORS-1) return;
    nPoint++;
  }
  for(int i=0;i<ALLVECTORS;i++) {qDebug()<<data[0][i]<<data[1][i]<<nPoint;}
  double y=data[1][nPoint-1],dy=(double)(data[1][nPoint]-data[1][nPoint-1])*(double)stepTime/(double)(data[0][nPoint]-data[0][nPoint-1]);
  while(t<data[0][ALLVECTORS-1]+0.001){
    if(t>=data[0][nPoint]-0.001) {
      nPoint++;
      dy=(double)(data[1][nPoint]-data[1][nPoint-1])*(double)stepTime/(double)(data[0][nPoint]-data[0][nPoint-1]);
    }//qDebug()<<"np"<<nPoint<<t<<data[0][nPoint]/TIMECONV+dt/100;
    xGr.append(t);
    if(!interpol) yGr.append(data[1][nPoint-1]);
    else{
      yGr.append(y);
      y=y+dy;
    }
//    qDebug()<<xGr.last()<<yGr.last()<<nPoint<<t<<y;
    t+=dt;
    ng++;
  }
// last data is zero
  xGr.append(t);
  yGr.append(0);
  ng++;
  plotRF->setDataForGraph(0,ng,qRgb(255,0,0),xGr.data(),yGr.data());
  plotRF->setAxis();
  plotRF->replotGraph();
  plotRF->repaint();
}

// slot save data
void Main_TD::slot_saveDataFile()
{
  QString fileName = QFileDialog::getSaveFileName(this,tr("Save data file"), dataFN, tr("Data file (*.dat)"));
  if(!fileName.isEmpty()) {
    dataFN=fileName;
    slot_plotGraph();
    slot_writeSettings();
  }
}

// slot read data
void Main_TD::slot_readDataFile()
{
  QFileInfo wf(dataFN);
  QString fileName = QFileDialog::getOpenFileName(this,
       tr("Open data file"), wf.canonicalPath(), tr("Data file (*.dat)"));
  if(!fileName.isEmpty()) {
    dataFN=fileName;
    slot_readSettings();
    putDataToTable();
    modifyData=false;
    stackDataX.clear();
    stackDataX.clear();
    undo_btn->setEnabled(false);
    slot_plotGraph();
    slot_writeSettings();
  }
}
// save csv file
void Main_TD::slot_saveCSVDataFile(void)
{
  status_Label->setText(" Status: Write data");
  QString outFileName;//=QFileDialog::getSaveFileName(this,tr("Save data file"), dataFNCSV, tr("Data file (*.csv)"));
  outFileName="workcsv.csv";
  if(!outFileName.isEmpty()) {
    dataFNCSV=outFileName;
    slot_plotGraph();
    slot_writeSettings();
    QFile file(outFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QMessageBox msgBox;
      msgBox.setText("Can't open file "+outFileName+" on write. May be file lock another application.");
      msgBox.exec();
      return;
    }
    QTextStream out(&file);

    out << "t,y1e,y2e,y1o,y2o\n";
    double y;
    for(int i=0;i<xGr.size();i++){
      y=yGr.at(i)*coefTransf;
      out.setRealNumberPrecision(5); out<<fixed<<xGr.at(i)/TIMECONV<<',';
      out.setRealNumberPrecision(1); out<<y<<','<<y<<','<<y<<','<<y<<'\n';
    }
  }
  savecsv_btn->setEnabled(false);
  timer->stop();
  alarmWrTimer->start(14000);
  http->putFileOnDevice(outFileName,outFileName);
  int err=http->getError();
  if (err==ERR_EMPTY) {
    err=ERR_NONE;
    status_Label->setText(" Status: Network return empty string.");
  }
  if(err) QMessageBox::warning(this,"error",http->getStrError());
  timer->start(2000);
}

void Main_TD::slot_alarmWriteAnswer()
{
  alarmWrTimer->stop();
  timer->stop();
  status_Label->setText(" Status: Data don't writen");
  err_Label->setText(" Error: HW error");
  QMessageBox::warning(this,"error",tr("Can't write data into HW"));
  savecsv_btn->setEnabled(true);
  timer->start(2000);
}
void Main_TD::slot_multData()
{
  pushData();
  QList<QTableWidgetSelectionRange> list;
  list=tableRf->selectedRanges();
  if(list.isEmpty()){
    for(int i=0;i<ALLVECTORS;i++){
      data[1][i]=data[1][i]*multCoeffSB->value();
    }
  }
  else {
    int j=list.at(0).leftColumn();
    int k=list.at(0).rightColumn();
    for(int i=j;i<=k;i++){
      data[1][i]=data[1][i]*multCoeffSB->value();
    }
  }
  putDataToTable();
  slot_plotGraph();
}

void Main_TD::slot_undoData()
{
  if(stackDataX.isEmpty()) return;
  popData();
  putDataToTable();
  slot_plotGraph();
  if(stackDataX.isEmpty()) {
    undo_btn->setEnabled(false);
  }
}
void Main_TD::popData(void)
{
  QVector <double> x,y;
  x=stackDataX.pop();
  y=stackDataY.pop();
  for(int i=0;i<ALLVECTORS;i++){
    data[0][i]=x.at(i);
    data[1][i]=y.at(i);
  }
  qDebug()<<"POP"<<data[1][1];
  modifyData=false;
}
void Main_TD::pushData(void)
{
  QVector<double> x,y;
  for(int i=0;i<ALLVECTORS;i++){
    x.append(data[0][i]);
    y.append(data[1][i]);
  }
  qDebug()<<"Push"<<data[1][1];
  stackDataX.push(x);
  stackDataY.push(y);
  undo_btn->setEnabled(true);
  modifyData=false;
}
