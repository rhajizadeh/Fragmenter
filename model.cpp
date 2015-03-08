#include "model.h"

#include <QThread>



Model::Model(Model *model)
{
    m_number = model->m_number;
    Atom *atom;
    for(int i=0;i<model->atomsCount();i++)
    {
        atom = new Atom(*model->atomAt(i));
        atoms.append(atom);
    }
    for(int i=0;i<model->m_chains.count();i++)
    {
        m_chains.append(new Chain(model->m_chains[i]));
    }
}

Model::Model(Model *model, QString endChain, int endResidue)
{
    m_number = model->m_number;
    Atom *atom;
    for(int i=0;i<model->atomsCount();i++)
    {
        atom = new Atom(*model->atomAt(i));
        atoms.append(atom);
    }
    for(int i=0;i<model->m_chains.count();i++)
    {
        m_chains.append(new Chain(model->m_chains[i]));
    }
    m_isProtein = true;

}


Model::~Model()
{
    xmin =0;
    xmax =0;
    ymin =0;
    ymax =0;
    zmax = 0;
    zmin = 0;
    Chain *chain;
    while(m_chains.count()>0)
    {
        chain = m_chains.takeFirst();
        delete chain;
    }

    Atom *atom;
    while(atoms.count()>0)
    {
        atom = atoms.takeFirst();
        delete atom;
    }

}


void Model::addAtom(Atom *atom)
{
    //    int i = m_chains.indexOf(atom->chain());
    //    if(i==-1)
    //    {
    //        m_chains.append(atom->chain());
    //    }

    updateRange(atom->x(),atom->y(),atom->z());
    atoms.append(atom);

}

Residue *Model::residueAt(int i)
{
    Residue *residue;
    for(int j=0;j<m_chains.count();j++)
    {
        for(int k=0;k<m_chains[j]->residues().count();k++)
        {
            residue = m_chains[j]->residues()[k];
            i--;
            if(i==0)
                return residue;
        }

    }
    return 0;
}

int Model::residueCount()
{
    int c=0;
    foreach (Chain *chain, m_chains) {
        c+= chain->residues().count();
    }
    return c;
}

int Model::getStartRes()
{
    return m_chains[0]->residues()[0]->serial();
}

//void Model::addResidue(Residue *residue)
//{

//    m_residues.append(residue);
//    if(!residue->isAminoAcid())
//        m_isProtein = false;

//    if(startRes == 0 ||  startRes > residue->serial())
//    {
//        startRes = residue->serial();
//    }
//}


void Model::calculateHBondEnergies(QList<Residue *> inResidues)
{
    for (quint32 i = 0; i + 1 < inResidues.size(); ++i)
    {
        Residue* ri = inResidues[i];

        for (quint32 j = i + 1; j < inResidues.size(); ++j)
        {
            Residue* rj = inResidues[j];

            if (ri->getCA()->center().distanceToPoint(rj->getCA()->center()) < kMinimalCADistance)
            {
                Residue::calculateHBondEnergy(ri, rj);
                if (j != i + 1)
                    Residue::calculateHBondEnergy(rj, ri);
            }
        }
    }
}

void Model::calculateSurfaces()
{
    QList<Residue*> residues = getResidues();
    QList<Residue*>::Iterator it1;
    QList<Residue*> residues2;

    for(it1=residues.begin(); it1<residues.end(); it1++)
    {
        (*it1)->resetAccessibility();
        if((*it1)->isAminoAcid())
            residues2.append(*it1);
    }
    for(it1=residues2.begin(); it1<residues2.end(); it1++)
    {
        (*it1)->calculateSurface(residues2);

    }
//    calculateHBondEnergies(residues2);
}

void Model::addChain(Chain *newChain)
{
    m_chains.append(newChain);
}

Chain *Model::findChainById(QString id)
{
  for(int i=0;i<m_chains.count();i++)
  {
      if(m_chains[i]->id() == id)
          return m_chains[i];

  }
  return 0;
}

void Model::calculateSurfaces(int from, QString startChain , int to, QString endChain)
{
    QList<Residue*> residues = getResidues();
    QList<Residue*> residues2;
    QList<Residue*>::Iterator it1;
    int i=1, counter = 0;
    bool endCheck=false;

    bool startChainCheck = startChain=="";
    bool endChainCheck = endChain=="";

    for(it1=residues.begin(); it1<residues.end(); it1++, counter++)
    {

        (*it1)->resetAccessibility();

        if(endChain == (*it1)->parentChain()->id())
            endChainCheck = true;

        if(startChain == (*it1)->parentChain()->id())
            startChainCheck = true;

        if((endCheck && endChainCheck) || !(counter>=from && startChainCheck))
            continue;
        if((*it1)->isAminoAcid())
            residues2.append(*it1);
        endCheck = i==to;

        i++;
    }
    //if(residues2.count() > toCountNames.count()){
        toCountNames.clear();
        foreach (Residue *r, residues2) {
            toCountNames.append(r->name());
        }

//    }
    for(it1=residues2.begin(); it1<residues2.end(); it1++)
    {
        (*it1)->calculateSurface(residues2);
    }
//    calculateHBondEnergies(residues2);
}

QList<Residue *> Model::getResidues()
{
    QList<Residue *> residues;
    foreach (Chain *chain, m_chains) {
        residues << chain->residues();
    }
    return residues;
}

void Model::updateRange(qreal x, qreal y, qreal z)
{
    //    qDebug() << xmin << xmax;
    xmin = xmin > x ? x : xmin;
    //    qDebug() << xmin << x << y << z;
    xmax = xmax < x ? x : xmax;

    ymin = ymin > y ? y : ymin;
    ymax = ymax < y ? y : ymax;

    zmin = zmin > z ? z : zmin;
    zmax = zmax < z ? z : zmax;

}

bool Model::isProtein() const
{
    return m_isProtein;
}

void Model::setIsProtein(bool isProtein)
{
    m_isProtein = isProtein;
}
