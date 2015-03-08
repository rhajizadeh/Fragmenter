#include "calculator.h"

#include <QThreadPool>
int Calculator::progress = 0;
Calculator::Calculator(QList<EndPoint> endPoints, QList<Group> groups, Model *model, QTextStream *stream,Modes mode, QObject *parent) :
    QThread(parent),
    m_endPoints(endPoints),
    m_groups(groups),
    m_model(model),
    m_stream(stream->device()),
    m_mode(mode),
    workingThread(0)
{
}

void Calculator::run()
{
    resetProgress();
    resaults().clear();
    iteration = 0;
    QThreadPool *pool = QThreadPool::globalInstance();
    QList<CalculatorRunnable *> threads;
    CalculatorRunnable *cal = 0;

    //freeModelsLock.release(2*idealThreadCount());
  //  qDebug() << freeModelsLock.available();

    for(int i=0;i<2*idealThreadCount();i++)
    {
        Model *model = new Model(m_model);
        freeModels.append(model);
    }
    for(int i=0;i<m_endPoints.count();i++)
    {
       // freeModelsLock.acquire();
        freeModelsMutex.lock();
        if(freeModels.isEmpty())
        {
            freeModelsMutex.unlock();
            msleep(100);
            freeModelsMutex.lock();
            i--;

        }
        else if(m_endPoints[i].endResidue-m_model->getStartRes()>0)
        {
            cal = new CalculatorRunnable(i, m_endPoints[0], m_endPoints[i], freeModels.first(), m_groups, m_mode, this);
            freeModels.pop_front();

            cal->setTotal(m_endPoints.count());
            threads.append(cal);
            pool->start(cal);
            thrStarted();            
        }
        freeModelsMutex.unlock();

    }
    pool->waitForDone();
    countGroups();
    writeTableHeader();
    qSort(resaultList.begin(),resaultList.end());

    for(int i=0;i<resaultList.count();i++)
    {

        writeToFile(resaultList[i].endPoint,resaultList[i].counts);
        m_finalEndPoints <<resaultList[i].endPoint;
        m_resaults << resaultList[i].counts;
    }
    m_stream.flush();
}

void Calculator::countGroups()
{
    QString toWrite="In your selection there was:\r\n";

//    qDebug() << Model::toCountNames;
    for(int j=0;j<m_groups.count();j++)
    {
        int count = 0;
        for(int i=0;i<finalToCount.count();i++){

            if(m_groups[j].AminoAcids.contains(finalToCount[i]))
            {
                count ++;
//                break;
            }
        }
        toWrite += m_groups[j].name + ": " + QString::number(count) +
                + " " + QString::number(count*100.0/finalToCount.count(),'g', 4)+ "%\r\n";
    }

    toWrite += "-----------------\r\n";
     m_stream << toWrite;
}

QList<EndPoint> Calculator::getFinalEndPoints()
{
    return m_finalEndPoints;
}

void Calculator::showProgress(int total)
{
    mutex2.lock();
    progress++;
    //    cout
    cout <<  QString::number((progress*1.0/total)*100,'g',4).toStdString() << "% Completed   \r";
    cout.flush();
    mutex2.unlock();
}
QMutex Calculator::mutex2;
void Calculator::appendToResaults(ThreadResault resault)
{
    QMutexLocker locker(&mutex);
    resaultList.append(resault);

}

void Calculator::writeToFile(EndPoint endPoint, QList<double> counts)
{
    QString toWrite="";
    toWrite += QString::number(++iteration) + "\t"+
            endPoint.chainName  +
            " " +  QString::number(endPoint.endResidue) + "\t";

    for(int i=0;i<counts.count();i++)
    {
        toWrite.append(QString::number(counts[i]) + "\t");
    }
    toWrite.append("\r\n");
    //    outfile->open(QFile::Append);
    //    outfile->write(toWrite.toLatin1());
    //    outfile->close();
    m_stream << toWrite;
}

void CalculatorRunnable::run()
{
//echo
    m_model->calculateSurfaces(m_startPoint.endResidue-m_model->getStartRes(), m_startPoint.chainName,
                               m_endPoint.endResidue-m_model->getStartRes(), m_endPoint.chainName);
    error = !countGroups(m_endPoint);

    Calculator::showProgress(total);
    m_calculator->thrEnded(m_model->getToCount(), m_model);
}

void Calculator::thrStarted()
{
    workingThread++;
}

void Calculator::thrEnded(QList<QString> toCountList, Model *model)
{
    workingThread--;
    freeModelsMutex.lock();
    freeModels.append(model);
    if(finalToCount.count() < toCountList.count())
    {
        finalToCount.clear();
        finalToCount.append(toCountList);
    }
    freeModelsMutex.unlock();
}

bool CalculatorRunnable::countGroups(EndPoint endPoint)
{
    QList<double> count;
    for(int j=0;j<m_groups.count();j++)
    {
        int c = 0;
        int counter = 0;

        for(int k=0;k<m_groups[j].AminoAcids.count();k++)
        {
            bool check = false;
            for(counter=0;counter<m_model->getResidues().count() && !check;counter++)
            {
                if(m_model->getResidues()[counter]->serial() == endPoint.endResidue
                        && ( endPoint.chainName == "" || m_model->getResidues()[counter]->parentChain()->id() == endPoint.chainName))
                {
                    check = true;
                }
                if(m_model->getResidues()[counter]->name() == m_groups[j].AminoAcids[k])
                {
                    c+=m_model->getResidues()[counter]->accessibility();

                }
            }
            if(!check)
            {
                cout << "couldn't find "<< endPoint.endResidue  << " in chain " << endPoint.chainName.toStdString() << " as an end point!\n";
                return false;
            }

        }
        if(m_mode==Acc1)
            count.append(c);
        else
            count.append(c*1.0/counter);
    }
    ThreadResault resault;
    resault.id = m_id;
    resault.endPoint = m_endPoint;
    resault.counts = count;


    m_calculator->appendToResaults(resault);


    return true;

}


void Calculator::writeTableHeader()
{
    QString toWrite="#\tend\t";

    for(int i=0;i<m_groups.count();i++)
    {
        toWrite += m_groups[i].name + "\t";
    }
    toWrite += "\r\n";
    m_stream << toWrite;
}
