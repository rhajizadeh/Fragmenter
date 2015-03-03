#include "chain.h"
#include <QDebug>
Chain::Chain(QString id, QObject *parent) :
    QObject(parent), m_id(id)
{
    //    qDebug() << m_id;
}

Chain::Chain(Chain *chain)
{
    m_id = chain->m_id;
    for(int i=0;i<chain->m_residues.count();i++)
    {
//        qDebug() << chain->m_residues[i]->name();
        m_residues.append(new Residue(chain->m_residues[i]));
    }
}

Chain::~Chain()
{
    Residue *residue;
    while(m_residues.count()>0)
    {
        residue = m_residues.takeFirst();
        delete residue;
    }
}

QList<Residue *> Chain::residues()
{
    return m_residues;
}

void Chain::addResidue(Residue *residue)
{
    m_residues.append(residue);
}

bool Chain::operator==(Chain value)
{
    return value.id() == id();
}

bool Chain::operator!=(Chain value)
{
    return value.id() != id();
}

QString Chain::operator=(Chain value)
{
    return value.id();
}

Residue *Chain::findResidueBySerial(int serial)
{
    for(int i=0;i<m_residues.count();i++)
    {
        if(m_residues[i]->serial() == serial)
            return m_residues[i];
    }
    return 0;
}
