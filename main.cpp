#include "pdbparser.h"

#include <QCoreApplication>
#include <iostream>
#include <new>

#include "pdbparser.h"
#include "calculator.h"
//#include "chartbuilders.h"

using namespace std;


QString outPut;
QString pdbFileName;
QString confFile;
QString jpegFilesDir;
QFile *outfile;
QTextStream stream;
QList<Group> groups;
QString endChain="";
int lineLength=0;


struct Fragment{
    QString name;
    QList<EndPoint> endPoints;
    QList< QList<double> > resaults;
};

Modes mode = Acc1;

QList<Fragment> fragments;

bool parseArguments(int argc,char *argv[],QString &message);
bool parseConfigfile(QString &message);
void openFile();
void writeHeader();
inline void writeLine();
void writeTableHeader();

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);

    QString message;
    if(!parseArguments(argc, argv, message))
    {
        cout << message.toStdString() << "\n";
        return -1;
    }
    if(!parseConfigfile(message))
    {
        cout << message.toStdString() <<"\n";
        return -1;
    }
    openFile();
    PdbParser *parser = new PdbParser();
    parser->setFileName(pdbFileName);
    cout << "Parsing PDB file...\n";
    parser->start();
    parser->wait();
    if(!parser->isSuccessful())
    {
        cout  << "Error Parsing\n";
        return -1;
    }
    cout  << "Parsing done!\n";
    writeHeader();
    Calculator *cal;
    //ChartBuilder *chartBuilder;
    QList<int> chartData;
    QStringList labels;


    for(int i=0;i<fragments.count();i++)
    {
      //  chartBuilder = new ChartBuilder();
        cout  << "Calculating " << fragments[i].name.toStdString() << " ... \n";
        cout << fragments[i].endPoints.count() << " end point to calculate.\n";
       // stream << fragments[i].name << ": \r\n";

        stream.flush();
        cal = new Calculator(fragments[i].endPoints,groups,parser->models()[0],&stream,mode);
        cal->start();
        cal->wait();
        fragments[i].endPoints = cal->getFinalEndPoints();
        fragments[i].resaults = cal->resaults();
        writeLine();
        stream.flush();
        labels.clear();
        for(int j=0;j<fragments[i].endPoints.count();j++)
        {
            QString temp =fragments[i].endPoints[j].chainName + " " + QString::number(fragments[i].endPoints[j].endResidue);
            labels.append(temp);
        }
//        chartBuilder->setLabels(labels);
//        chartBuilder->setHorzontalLabel("ACC");
//        chartBuilder->setVerticalLabel("End Points");
        for(int k=0;k<groups.count();k++)
        {
            chartData.clear();

            for(int j=0;j<fragments[i].endPoints.count();j++)
            {
                chartData.append( fragments[i].resaults[j][k]);
            }
//            chartBuilder->addData(groups[k].name, chartData);
        }
//        if(jpegFilesDir!=""){
//            if(jpegFilesDir.at(jpegFilesDir.length()-1) != '/')
//                    jpegFilesDir += "/";
//            chartBuilder->saveImage(jpegFilesDir + fragments[i].name +".png");
//        }
//        else
//            chartBuilder->saveImage( fragments[i].name +".png");
        delete cal;
//        delete chartBuilder;
    }
    outfile->close();
    app.exit(EXIT_SUCCESS);
}


bool parseArguments(int argc,char *argv[],QString &message)
{

    QString current;
    int recognizedArguments = 0;
    for(int i=0;i<argc;i++)
    {
        current = QString::fromLatin1(argv[i]);
        if(current=="-o")
        {
            if(i<argc-1)
            {
                outPut = QString::fromLatin1(argv[i+1]);
                i++;
                recognizedArguments++;
            }
            else
            {
                message = "no output file";
                return false;
            }
        }
        else if(current=="-i")
        {
            if(i<argc-1)
            {
                pdbFileName = QString::fromLatin1(argv[i+1]);
                i++;
                recognizedArguments++;
            }
            else
            {
                message = "no input file";
                return false;
            }
        }
        else if(current=="-c")
        {
            if(i<argc-1)
            {
                confFile = QString::fromLatin1(argv[i+1]);
                i++;
                recognizedArguments++;
            }
            else
            {
                message = "no config file";
                return false;
            }
        }
        else if(current=="-j")
        {
            if(i<argc-1)
            {
                jpegFilesDir = QString::fromLatin1(argv[i+1]);
                //                i++;
                recognizedArguments++;
            }
            else
            {
                message = "no jpeg file";
                return false;
            }
        }
    }
    if(recognizedArguments<3)
    {
        message = " some arguments didnt entered.";
        return false;
    }
    return true;
}

bool parseConfigfile(QString &message)
{
    QFile file(confFile);
    if(!file.open(QFile::ReadOnly))
    {
        message = "error opening configuration file";
        return false;
    }
    QString temp(file.readAll());
    file.close();

    temp = temp.trimmed();

    QStringList lines = temp.split('\n');
    for(int i=0;i<lines.count();i++)
    {

        if(lines[i] == "" ||  lines[i].split(':').count()!=2)
            continue;
        QString s = lines[i].split(':')[0].trimmed();
        QString arg = lines[i].split(':')[1].trimmed();
        if(s=="")
            continue;
        if(arg=="")
        {
            message = "error in config file: argument of: " + s;
            return false;
        }
        if(s=="mode")
        { 
            if(arg=="1")
                mode = Acc1;
            else
                mode = Acc2;

        }
        else if(s.split(' ')[0]=="fgs")
        {
            Fragment newFrag;
            newFrag.name = s.split(' ')[1];
            QStringList list = arg.split(',');
            QStringList pointsString;

            for(int j=0;j<list.count();j++)
            {
                QString string = list[j].trimmed();
                QString chain = "";
                if(string.contains(' '))
                {
                    chain = string.split(' ')[0];
                    string = string.split(' ')[1];
                }
                if(string.contains('-'))
                {
                    bool check1 = false, check2 = false, check3 = false;
                    pointsString = string.split('-');
                    if(pointsString.length()!=3)
                    {
                        message = "error in config file: wrong end point";
                        return false;
                    }
                    int start = pointsString[0].toInt(&check1);
                    int end = pointsString[1].toInt(&check2);
                    int step = pointsString[2].toInt(&check3);
                    if(check1 && check2 && check3)
                    {
                        for(int i=start;i<=end;i+=step)
                        {
                            EndPoint newEndPoint;
                            newEndPoint.chainName = chain;
                            newEndPoint.endResidue = i;
                            newFrag.endPoints.append(newEndPoint);
                        }
                    }
                    else
                    {
                        message = "error in config file: wrong end point";
                        return false;
                    }
                }
                else
                {
                    int num=0;
                    bool check = false;

                    check = false;
                    num = string.toInt(&check);
                    if(!check)
                    {
                        message = "error in config file: wrong end point";
                        return false;
                    }
                    EndPoint newEndPoint;
                    newEndPoint.chainName = chain;
                    newEndPoint.endResidue = num;
                    newFrag.endPoints.append(newEndPoint);
                }

            }
            fragments.append(newFrag);
        }
        else
        {
            Group p = {s, arg.split(' ')};
            groups.append(p);
        }
    }
    return true;
}

void openFile()
{

    outfile = new QFile(outPut);
    outfile->remove();
    if(!outfile->open(QFile::WriteOnly))
        qDebug() << "Error opening output file";
    stream.setDevice(outfile);
}

void writeHeader()
{
    // outfile->open(QFile::Append);
    QString toWrite="";
    toWrite = "=== Surface ARea Analysis Tool (SARA) ======\r\n";
    lineLength=toWrite.count();
    for(int i=0;i<groups.count();i++)
    {
        toWrite += groups[i].name + ": ";
        for(int j=0;j<groups[i].AminoAcids.count();j++)
        {
            toWrite += groups[i].AminoAcids[j] + " ";
        }
        toWrite += "\r\n";
    }

    stream << toWrite;
    writeLine();

}

inline void writeLine()
{
    QString toWrite="";
    for(int i=0;i<lineLength;i++)
        toWrite += "-";
    toWrite += "\r\n";
    stream << toWrite;
}

