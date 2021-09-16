#include "httpProcess.h"
//#define WINVER WindowsXP

//#include <windows.h>

//typedef DWORD EXECUTION_STATE;
//extern "C" EXECUTION_STATE  SetThreadExecutionState(EXECUTION_STATE esFlag);
THttpProc::THttpProc()
{
  loop =new QEventLoop();
  manager = new QNetworkAccessManager(this);
  setDeviceAddress("127.0.0.1"); // set local
  uError="";
  //SetThreadExecutionState(EXECUTION_STATE,ES_DISPLAY_REQUIRED | EXECUTION_STATE,ES_CONTINUOUS);
  //qDebug()<<Qt::hex<<SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | 0x00000040);
}

THttpProc::~THttpProc()
{
  if(manager) {delete manager;manager=0;}
  if(loop) {delete loop;loop=0;}
  data.clear();
  //qDebug()<<Qt::hex<<SetThreadExecutionState(ES_CONTINUOUS);
}

//********************** Public functions ***********************
//-------------------- Get file list fl from input directory on the device. Return 0 if OK
int THttpProc::getStatus(QStringList &fl)
{
  QUrl url(SERVICE_URL);
  QNetworkRequest request(url);
  QByteArray postData("get_status");

  reqCompleate=false;
  error=ERR_NONE;

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  connect(manager, SIGNAL(finished(QNetworkReply*)),
           this, SLOT(replyFinishedNetworkReq(QNetworkReply*)));
  manager->post(request,postData);

  int tout=0;
  while(!reqCompleate) {
    loop->processEvents();
    msleep(50);
    if(tout++>TOUT_IN_S*20){
      error=ERR_TOUT; break;
    }
  }
  disconnect(manager,0,0,0);
  if(!error){
    QString st(data);
    fl.append(st.simplified().split(';'));
  }
  return error;
}

//--------------------Get file list fl from output directory on the device. Return 0 if OK
int THttpProc::getOutputListFiles(QStringList &fl)
{
  QUrl url(SERVICE_URL);
  QNetworkRequest request(url);
  QByteArray postData("list_output");

  reqCompleate=false;
  error=ERR_NONE;

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  connect(manager, SIGNAL(finished(QNetworkReply*)),
           this, SLOT(replyFinishedNetworkReq(QNetworkReply*)));
  manager->post(request,postData);

  int tout=0;
  while(!reqCompleate) {
    loop->processEvents();
    msleep(50);
    if(tout++>TOUT_IN_S*20){
      error=ERR_TOUT; break;
    }
  }
  disconnect(manager,0,0,0);
  if(!error){
    QString st(data);
    fl.append(st.simplified().split(' '));
  }
  return error;
}

//-------------------- Upload file fn on device in input directory with filename fns. Return 0 if OK
int THttpProc::putFileOnDevice(QString fn,QString fns)
{
  QFile fileRd(fn);
  if(!fileRd.exists()){
    error=ERR_FEXIST;
    return(error);
  }
  QUrl url(SERVICE_URL);
  QNetworkRequest request(url);
  QString bound="margin"; //name of the boundary

  reqCompleate=false;
  error=ERR_NONE;

  if(!fileRd.open(QIODevice::ReadOnly)){
    error=ERR_FOPEN;
    return(error);
  }

  connect(manager, SIGNAL(finished(QNetworkReply*)),
           this, SLOT(replyFinishedNetworkReq(QNetworkReply*)));

 //according to rfc 1867 we need to put this string here:
  QByteArray postData(QString("--" + bound + "\r\n").toLatin1());//according to rfc 1867

  postData.append(QString("Content-Disposition: form-data; name=\"uploaded\"; filename=\""+fns+"\"\r\n").toLatin1());
  //name of the input is "uploaded" in my form, next one is a file name.
  postData.append("Content-Type: data/rt\r\n\r\n"); //data type

  postData.append(fileRd.readAll());//let's read the file
  fileRd.close();

  postData.append("\r\n");
  postData.append(QString("--" + bound + "--\r\n").toLatin1()); //closing boundary according to rfc 1867

  request.setRawHeader(QString("Content-Type").toLatin1(),QString("multipart/form-data; boundary=" + bound).toLatin1());
  request.setRawHeader(QString("Content-Length").toLatin1(), QString::number(postData.length()).toLatin1());

  manager->post(request,postData);

  int tout=0;
  while(!reqCompleate) {
    loop->processEvents();
    msleep(50);
    if(tout++>TOUT_IN_S*20){
      error=ERR_TOUT; break;
    }
  }

  disconnect(manager,0,0,0);
  if(!error){
    bool ok=false;
    int ret=data.toInt(&ok);
    if(ok){
      if(ret) error=ERR_OPERATION;
    }
    else{
      uError=data;
      error=ERR_UANSWER;
    }
  }
  return error;
}

//--------------------  Get file fn from Output directory and save one as fns. Return 0 if OK
int THttpProc::getOutputFileFromDevice(QString fn,QString fns)
{
  QUrl url(SERVICE_URL);
  QNetworkRequest request(url);
  QByteArray postData("get_output=");
  postData.append(fn.toLatin1());

  reqCompleate=false;
  error=ERR_NONE;

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  connect(manager, SIGNAL(finished(QNetworkReply*)),
           this, SLOT(replyFinishedNetworkReq(QNetworkReply*)));

  manager->post(request,postData);
  int tout=0;
  while(!reqCompleate) {
    loop->processEvents();
    msleep(50);
    if(tout++>TOUT_IN_S*20){
      error=ERR_TOUT; break;
    }
  }
  disconnect(manager,0,0,0);
  if(!error){
    QFile fileWr(fns);
    if(!fileWr.open(QIODevice::WriteOnly)) {
      error=ERR_FWRITE;
      return error;
    }
    fileWr.write(data);
    fileWr.close();
  }
  return error ;
}

//-------------------- Delete file fn from input directory. Return 0 if OK
int THttpProc::removeInputFile(QString fn)
{
  QUrl url(SERVICE_URL);
  QNetworkRequest request(url);
  QByteArray postData("remove_input=");
  postData.append(fn.toLatin1());

  reqCompleate=false;
  error=ERR_NONE;

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  connect(manager, SIGNAL(finished(QNetworkReply*)),
           this, SLOT(replyFinishedNetworkReq(QNetworkReply*)));

  manager->post(request,postData);

  int tout=0;
  while(!reqCompleate) {
    loop->processEvents();
    msleep(50);
    if(tout++>TOUT_IN_S*20){
      error=ERR_TOUT;  break;
    }
  }
  disconnect(manager,0,0,0);
  if(!error){
    bool ok=false;
    int ret=data.toInt(&ok);
    if(ok){
      if(ret) error=ERR_OPERATION;
    }
    else {
      uError=data;
      error=ERR_UANSWER;
    }
  }
  return error;
}

//-------------------- Delete file fn from output directory. Return 0 if OK
int THttpProc::removeOutputFile(QString fn)
{
  QUrl url(SERVICE_URL);
  QNetworkRequest request(url);
  QByteArray postData("remove_output=");
  postData.append(fn.toLatin1());

  reqCompleate=false;
  error=ERR_NONE;

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  connect(manager, SIGNAL(finished(QNetworkReply*)),
           this, SLOT(replyFinishedNetworkReq(QNetworkReply*)));

  manager->post(request,postData);

  int tout=0;
  while(!reqCompleate) {
    loop->processEvents();
    msleep(50);
    if(tout++>TOUT_IN_S*20){
      error=ERR_TOUT;  break;
    }
  }
  disconnect(manager,0,0,0);
  if(!error){
    bool ok=false;
    int ret=data.toInt(&ok);
    if(ok){
      if(ret) error=ERR_OPERATION;
    }
    else {
      uError=data;
      error=ERR_UANSWER;
    }
  }
  return error;
}

// Convert error code into string
QString THttpProc::getStrError(void)
{
  switch(error){
    case ERR_NONE:       return "";
    case ERR_TOUT:       return tr("Timeout network operation. Can't recieved answer from device.");
    case ERR_EMPTY:      return tr("Device return empty answer.");
    case ERR_OPERATION:  return tr("Can't execute operation.");
    case ERR_UANSWER:    return (tr("Device return unknown answer... ")+uError);
    case ERR_FEXIST:     return tr("File not exists.");
    case ERR_FOPEN:      return tr("File open I/O error.");
    case ERR_FWRITE:     return tr("File write I/O error.");
    case ERR_NFPHP:      return tr("Execute PHP file not found on device.");
    default:             return tr("");
  }
}

//********************** Private SLOTS functions ***********************
// answer on request from device
void THttpProc::replyFinishedNetworkReq(QNetworkReply *reply)
{
  data.clear();
  data = reply->readAll();
qDebug()<<"DATA"<<data;
  if(data.isEmpty()) error=ERR_EMPTY;
  if(data[0]=='-') error=ERR_OPERATION; // operation not execute
  if(data.startsWith("File not found")) error=ERR_NFPHP;
  reqCompleate=true;
}


