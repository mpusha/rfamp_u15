#ifndef HTTPPROCESS_H
#define HTTPPROCESS_H

#include <QtDebug>
#include <QThread>
#include <QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QTextCodec>
#include <QFile>
#include <QDir>

#define TOUT_IN_S 5 // 5 sec timeout on network operation
#define ERR_NONE       0
#define ERR_TOUT       1
#define ERR_EMPTY      2
#define ERR_OPERATION  3
#define ERR_UANSWER    4
#define ERR_FEXIST     5
#define ERR_FOPEN      6
#define ERR_FWRITE     7
#define ERR_NFPHP      8



class THttpProc :public QThread
{
  Q_OBJECT

public:
  explicit THttpProc();
  ~THttpProc();

   int getError(void){return error;} // return code of error 0 if OK
   QString getStrError(void); // return description of error

   void setDeviceAddress(QString str){SERVICE_URL="http://"+str+"/rfamp/data/wr.php";}
   int getOutputListFiles(QStringList&);  // Contents of Output directory on the device

   int putFileOnDevice(QString fn,QString fns); // Put file fn on device in data directory
   int getStatus(QStringList&);   //Get status and error

   int getOutputFileFromDevice(QString fn,QString fns); // Get file fn from Output directory and save one as fns.
   int removeInputFile(QString fn); // remove file fn from input directory
   int removeOutputFile(QString fn); // remove file fn from output directory

private slots:
  void replyFinishedNetworkReq(QNetworkReply*); // slot on reply from device
private:
  QEventLoop *loop;
  QString SERVICE_URL;
  QNetworkAccessManager  *manager;

  bool reqCompleate;  // true after request
  int error;          // error code
  QString uError;     // return string when unknown answer recieved
  QByteArray data;    // data with answer from device

};
#endif // HTTPPROCESS_H
