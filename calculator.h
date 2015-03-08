#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QFile>
#include <QRunnable>
#include <QThread>
#include <QQueue>
#include <iostream>
#include <QSemaphore>
#include "model.h"


using namespace std;


struct Group
{
    QString name;
    QStringList AminoAcids;
};

struct EndPoint{
    int endResidue;
    QString chainName;
};


struct ThreadResault{
    int id;
    EndPoint endPoint;
    QList<double> counts;
    bool operator <(const ThreadResault resault) const
    {
        return id < resault.id;
    }
};

enum Modes{Acc1 //normal
           ,Acc2 //divieded
          };


class Calculator;
class Model;
class CalculatorRunnable: public QRunnable
{
public:
    CalculatorRunnable(int id,EndPoint startPoint, EndPoint endPoint,Model *model,QList<Group> groups,Modes mode,Calculator *calculator)
    {
        m_endPoint = endPoint;
        m_startPoint = startPoint;
        m_groups = groups;
        m_mode = mode;
        m_id = id;
        total = -1;
        m_model = model;
        m_calculator = calculator;
        setAutoDelete(true);
    }
    ~CalculatorRunnable()
    {
      //  delete m_model;
    }

    void run();
    void setTotal(int value){   total = value;      }
    bool isSuccessful()     {   return !error;      }
    int id()                {   return m_id;        }
    EndPoint endPoint()     {   return m_endPoint;  }
//    QList<int> resaults()   {   return m_resaults;  }

private:
    EndPoint m_endPoint, m_startPoint;
    QString m_chainName;
    Model *m_model;
    Modes m_mode;
    QList<Group> m_groups;
    bool countGroups(EndPoint endPoint);
    bool error;
    QList<int> m_resaults;
    int m_id;
    int total;

    Calculator *m_calculator;
};

class Calculator : public QThread
{
    Q_OBJECT
public:
    explicit Calculator(QList<EndPoint> endPoints, QList<Group> groups, Model* model,  QTextStream *stream,Modes mode, QObject *parent = 0);
    QList<QList<double> > resaults()
    { return m_resaults; }
    QList<EndPoint> getFinalEndPoints();
    static void showProgress(int total);
    static void resetProgress(){progress = 0;}
    void appendToResaults(ThreadResault resault);
    void thrStarted();
    void thrEnded(QList<QString> toCountList, Model *model);
protected:
    void run();
    void countGroups();
    void writeTableHeader();
private:
    QQueue<Model *> freeModels;
    QMutex freeModelsMutex;
    QList<EndPoint> m_endPoints,m_finalEndPoints;
    QList<Group> m_groups;
    QList< QList<double> > m_resaults;
    Model *m_model;
    QList<int> count;
    //    QFile *outfile;
//    bool countGroups(EndPoint endPoint);
    void writeToFile(EndPoint endPoint, QList<double> counts);
    Modes m_mode;
    int iteration;
    QTextStream m_stream;
    static int progress;
    int workingThread;
    QMutex mutex;
    static QMutex mutex2;
    QList<ThreadResault> resaultList;
    QList<QString> finalToCount;
signals:
    
public slots:
    
};

#endif // CALCULATOR_H
