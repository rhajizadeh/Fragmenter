#include "pdbparser.h"
#include <QDebug>
#include <qmath.h>
#include <iostream>

using namespace std;
PdbParser::PdbParser(QObject *parent) :
    QThread(parent)
{
    currModel = 1;
    currentResidue = 0;
    currChain = 0;
    error = false;
}
void PdbParser::setFileName(QString fileName)
{
    this->fileName = fileName;
    file = new QFile(fileName);
}

int PdbParser::parseModel(QString line)
{
    return  line.mid(10,4).simplified().toInt();
}

Atom* PdbParser::parseAtom(QString line, Model *model)
{
    int serial = line.mid(6,5).simplified().toInt();
    QString name = line.mid(12,4).simplified();
    float x = line.mid(30,8).simplified().toFloat();
    float y = line.mid(38,8).simplified().toFloat();
    float z = line.mid(46,8).simplified().toFloat();
    Atom *newAtom = new Atom(serial, name, x, y, z);

    return newAtom;
}

void PdbParser::parseBonds(Atom *atom, Model *model)
{

}


void PdbParser::run()
{
    clearLists();
    if(!file->exists() || !file->open(QFile::ReadOnly))
    {
        message = "could not open pdb file in " + fileName;
        error = true;
        return;
    }
    if(file->size()==0)
    {
         message =  "file is empty";
        error = true;
        return;
    }

    QString line,lineType;
    int nLines=0,ln=0;
    Atom *newAtom=0;
    Model *newModel=0;
    Chain *tempChain = 0;
    while(!file->atEnd())
    {
        file->readLine();
        nLines++;
    }
    file->seek(0);
    currModel = 1;
    newModel = new Model(currModel);
    modelList.append(newModel);

    while(!file->atEnd())
    {
        line =file->readLine();
        lineType = line.left(6);
        lineType = lineType.simplified();

        if(lineType == "TITLE")
        {
            m_title = line.mid(10).simplified();
        }
        else if(lineType == "HEADER")
        {
            m_classification = line.mid(10,39).simplified();
        }
        else if(lineType=="MODEL")
        {
            int m = parseModel(line);
            if(currModel!=m)
            {
                currModel = m;
                newModel = new Model(currModel);
                modelList.append(newModel);
            }
            //          qDebug() << "model" << currModel;
        }
        else if(lineType=="ATOM" /*|| lineType == "HETATM"*/)
        {
            if(newModel==0)
            {
                currModel = parseModel(line);
                currChain = 0;
                newModel = new Model(currModel);
                modelList.append(newModel);
            }
            tempChain = new Chain(line.mid(21,1).trimmed());
//            qDebug() << line.mid(21,1);

            if(currChain == 0 || tempChain->id().trimmed()!=currChain->id().trimmed())
            {
                currChain = tempChain;
                newModel->addChain(currChain);
            }
            else
                delete tempChain;

            if(!parseResidue(line, newModel))
            {
                qDebug() << line;
                error = true;
                return;
            }
            newAtom = parseAtom(line,newModel);
            if(newAtom==NULL)
                continue;
            currentResidue->addAtom(newAtom);
            newModel->addAtom(newAtom);
            countAtom(newAtom);

        }
        emit readProgress(1,nLines);
        ln++;
    }

    if(!currentResidue->finalizeResidue(message))
    {
        cout << message.toStdString();
        error = false;
        return;
    }
    file->close();

    emit parsingDone();
}



void PdbParser::clearLists()
{
    Model *model;
    while (modelList.count()>0)
    {
        model = modelList.takeFirst();
        delete model;
    }
    modelList.clear();
}

bool PdbParser::parseResidue(QString line, Model *model)
{
    int resSerial = line.mid(22,4).simplified().toInt();
    QString resName = line.mid(17,3).simplified();

    if( currentResidue == 0 )
    {
        currentResidue = new Residue(resSerial,resName,currChain);
        //model->addResidue(currentResidue);
        currChain->addResidue(currentResidue);
        countResidue(currentResidue);

    }
    else if( currentResidue->serial() != resSerial /*|| currentResidue->name() != resName*/)
    {
        if(!currentResidue->finalizeResidue(message))
        {
            cout << message.toStdString();
            return false;
        }
        currentResidue = new Residue(resSerial,resName,currChain,currentResidue);
        //model->addResidue(currentResidue);
        currChain->addResidue(currentResidue);
         countResidue(currentResidue);
    }
    return true;
}

void PdbParser::countAtom(Atom *atom)
{
    int i = m_atomsIncluded.indexOf(atom->atomName());
    if(i==-1)
    {
        m_atomsIncluded.append(atom->atomName());
        m_atomCount.append(0);
    }
    else
    {
        m_atomCount.replace(i ,m_atomCount.at(i)+1);
    }



    if(atom->name() == "CA" || atom->name() == "C"
            || atom->name() == "N" || atom->name() == "O" )
    {
        i = m_backboneAtomsIncluded.indexOf(atom->atomName()
                                            + " " + atom->getType());
        if(i==-1)
        {
            m_backboneAtomsIncluded.append(atom->atomName()
                                           + " " + atom->getType());
            m_atomsInBackboneCount.append(0);
        }
        else
        {
            m_atomsInBackboneCount.replace(i ,m_atomsInBackboneCount.at(i)+1);
        }
    }
}

void PdbParser::countResidue(Residue *residue)
{
    int i = m_residuesIncluded.indexOf(residue->completeName());
    if(i==-1)
    {
        m_residuesIncluded.append(residue->completeName());
        m_residueCount.append(0);
    }
    else
    {
        m_residueCount.replace(i ,m_residueCount.at(i)+1);
    }
}

int PdbParser::getaAtomCount(QString name)
{
    int i = m_atomsIncluded.indexOf(name);
    if(i==-1)
        return 0;
    return m_atomCount.at(i);
}

int PdbParser::getaAminoCount(QString name)
{
    int i = m_residuesIncluded.indexOf(name);
    if(i==-1)
        return 0;
    return m_residueCount.at(i);
}


