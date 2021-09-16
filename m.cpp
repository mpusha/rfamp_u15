#include <QtCore/QCoreApplication>
#include "httpProcess.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    THttpProc *http;
    http=new THttpProc;
    QStringList list;
    http->setDeviceAddress("172.18.0.12");  // set device address
    while(1){
// get list of input orders
      if(http->getInputListFiles(list)) {qDebug()<<"Error! "<<http->getStrError(); break;}
      qDebug()<<"Input orders list";
      for(int i=0;i<list.size();i++){
        qDebug()<<list.at(i);
      }
      qDebug();
      list.clear();
 // get list of output orders
      if(http->getOutputListFiles(list)) {qDebug()<<"Error! "<<http->getStrError(); break;}
      qDebug()<<"Output orders list";
      for(int i=0;i<list.size();i++){
        qDebug()<<list.at(i);
      }
      qDebug();
// get file 1.rt from output dir and save as 1.r
      qDebug()<<"Get output order";
      if(http->getOutputFileFromDevice("run.rt","1.r")) {qDebug()<<"Get output order error! "<<http->getStrError(); break;}
      qDebug()<<"Get output order compleate.";
      qDebug();
// put file 1.r in input dir by name 1.rt
      qDebug()<<"Upload input order";
      if(http->putInputFileOnDevice("1.r","1.rt")) {qDebug()<<"Upload input order error! "<<http->getStrError(); break;}
      qDebug()<<"Upload input order compleate.";
      qDebug();
// remove file 1.rt from input dir
      qDebug()<<"Remove input order";
      //if(http.removeInputFile("1.rt")) {qDebug()<<"Error! "<<http.getStrError(); break;}
      qDebug()<<"Remove input order compleate.";
      qDebug();
  // remove file 1.rt from output.dir
      qDebug()<<"Remove output order";
     // if(http.removeOutputFile("1.rt")) {qDebug()<<"Error! "<<http.getStrError(); break;}
      qDebug()<<"Remove output order compleate.";
      qDebug();
      break;
    }
    qDebug()<<"End!";
    http->terminate();
    delete http;
    //return a.exec();
}
