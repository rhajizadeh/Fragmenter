#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QList>
#include <QStringList>
#include "chain.h"
#include "atom.h"
#include "residue.h"

class Atom;
class Chain;
class Bond;
class Residue;
class Model
{

public:

    Model(int number)
    {
        m_number = number;
    }
    Model(Model *model,QString endChain,int endResidue);
    Model(Model *model);
    ~Model();
    void addAtom(Atom *atom);
    void addBond(Bond *bond)
    {bonds.append(bond);}
//    void addResidue(Residue *residue);

    Bond *bondAt(int i)
    {return bonds[i];}
    Atom *atomAt(int i)
    {return atoms[i];}

    QList<Atom *> atomList(){return atoms;}
    Residue * residueAt(int i);
    Chain *getChainAt(int i)
    {
        return m_chains[i];
    }

    int atomsCount(){return atoms.count();}
    int bondsCount(){return bonds.count();}
    int residueCount();
    int number(){return m_number;}
    int getStartRes();


    QList<Residue *> getResidues();

    QList<Chain*> chains() {return m_chains;}

    inline qreal getXMin() { return xmin; }
    inline qreal getYMin() { return ymin; }
    inline qreal getZMin() { return zmin; }
    inline qreal getXMax() { return xmax; }
    inline qreal getYMax() { return ymax; }
    inline qreal getZMax() { return zmax; }

    bool isProtein() const;
    void setIsProtein(bool isProtein);
    void calculateHBondEnergies(QList<Residue *> inResidues);
    void calculateSurfaces(int from, QString startChain, int to, QString endChain);
    void calculateSurfaces();
    void addChain(Chain *newChain);

    Chain *findChainById(QString id);
    QList<QString> getToCount(){ return toCountNames; }
    //void calculateHBondEnergies(QList<Residue *> inResidues);
protected:
    void updateRange(qreal x, qreal y, qreal z);
private:
    QList<QString> toCountNames;
    QList<Atom *> atoms;
    QList<Bond *> bonds;
//    QList<Residue *>m_residues;
    QList<Chain*> m_chains;

    int m_number;
     qreal xmin,xmax;
     qreal ymin,ymax;
     qreal zmin,zmax;

     bool m_isProtein;

public slots:

};

#endif // MODEL_H
