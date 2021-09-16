#ifndef MAIN_TD_H
#define MAIN_TD_H


#include <QtWidgets>
#include "httpProcess.h"
#include "data_plot.h"

//------------------------------------------------------
#define MAXTIME 500
#define MINTIME 100
#define ALLVECTORS 32
#define TIMECONV 1e3
#define HEADER_COLOR  0xfff6f7f9

const double maxTime=40.0;
const double maxAmp=10.0;
//------------------------------------------------------
class Main_TD;
//------------------------------------------------------
class Main_TD : public QMainWindow
{
    Q_OBJECT

public:
//--- Constructor&Destructor ---
    Main_TD(QWidget *parent = 0);
    ~Main_TD();
    void putDataToTable(void);
    void getDataFromTable(void);
    void sortData(void);

private:
//--- Widgets ---
  QVBoxLayout *main_layout;
  QHBoxLayout *edit_layout,*data_layout,*graph_layout;
  QGroupBox *MainGroupBox;            // Main Box
  QPushButton *refresh_btn,*save_btn,*savecsv_btn,*read_btn,*mult_btn;
  QLabel *space_Label,*time1_Label,*workfile_label,*transf_Label,*status_Label,*err_Label,*multCoeffLabel;
  QTableWidget *tableRf;
  QCheckBox *interpolCB;
  QDoubleSpinBox *multCoeffSB;
  QLabel *mode_label;             // Label -> StatusBar
  QMenu *file_menu;
  QAction *exit;
  QFont app_font;                 // Font
  TDataPlot *plotRF;

  QTimer *timer,*alarmWrTimer;                  // Timer for time update
  QMessageBox msgBox;

private:
//--- Methods ---
    void create_ListWidget();
    void create_StatusBar();        // create StatusBar
    void create_Menu();             // create Menu
    void Sleep(int ms);
    void keyPressEvent(QKeyEvent *event);
//--- Widgets ---


private:
//--- Data ----
    QTranslator translator;
    QString LanguageApp;            // language application

    QTableWidgetItem *itemTable[2][32];
    QString httpAddress;
    double stepTime;
    double coefTransf;
    QString dataFN,dataFNCSV; // file name for RF (time and RF)
    double data[2][ALLVECTORS];
    QVector <double> xGr,yGr; // for plot graph
    uint32_t interpol;
    THttpProc *http;

public slots:

    void slot_updateDateTime(void);
    void slot_readSettings(void);
    void slot_writeSettings(void);
    void slot_plotGraph(void);
    void slot_saveDataFile(void);
    void slot_readDataFile(void);
    void slot_saveCSVDataFile(void);
    void slot_alarmWriteAnswer(void);
    void slot_multData(void);

protected:


};

#endif // MAIN_TD_H
