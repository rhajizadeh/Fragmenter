#ifndef AMINOACID_H
#define AMINOACID_H

#include <QObject>
//#include <QtGui/QColor>
//#include <QtGui/QVector3D>
#include <qmath.h>
#include "chain.h"
#include "atom.h"
#include "vector3d.h"

const double
    kRadiusN = 1.65,
    kRadiusCA = 1.87,
    kRadiusC = 1.76,
    kRadiusO = 1.4,
    kRadiusSideAtom = 1.8,
    kRadiusWater = 1.4;

const double
    kSSBridgeDistance = 3.0,
    kMinimalDistance = 0.5,
    kMinimalCADistance = 9.0,
    kMinHBondEnergy = -9.9,
    kMaxHBondEnergy = -0.5,
    kCouplingConstant = -27.888,	//	= -332 * 0.42 * 0.2
    kMaxPeptideBondLength = 2.5;

using namespace std;

class Atom;
class Accumulator;
class Chain;
class Residue
{
    struct HBond
    {
        Residue*		residue;
        double			energy;
    };
public:
    explicit Residue(int serial,QString name,Chain *parentChain,Residue *m_pre=0);
    Residue (Residue *residue);
    ~Residue();
    QString name(){return m_name;}
    QString completeName(){return m_compeleteName;}
    int serial(){return m_serial;}
    //QColor color(){return m_color;}
    bool selected() const;
    void setSelected(bool selected);
    void addAtom(Atom *atom);
    bool finalizeResidue(QString &message);
    void addNeighbours(Residue *neighbour);
    void calculateSurface(QList<Residue*> inResidues);
    static double calculateHBondEnergy(Residue *inDonor, Residue *inAcceptor);

    HBond*	Donor()	{ return mHBondDonor; }
    HBond*	Acceptor() { return mHBondAcceptor; }

    Chain *parentChain()
    {   return m_parent;    }

    QList<Atom *> getAllAtoms();
    QStringList getAllAtomsNames();
    QList<Atom *> getBackboneAtoms();
    Atom *getCA(){return m_CA;}
    Atom *getN(){return m_N;}
    Atom *getO(){return m_O;}
    Atom *getC(){return m_C;}
    Atom *getH(){return m_H;}

    Vector3D getCenter(){return m_Center;}
    qreal getRadius(){return m_Radius;}
    qreal accessibility() const;

    double Kappa() const;
    double TCO() const;
    double Alpha();
    double Phi() const;
    double Psi() const;


    bool isAminoAcid() const;
    void setIsAminoAcid(bool isAminoAcid);

    void resetAccessibility();
protected:
    void extendBox(Atom *atom, int inRadius);
    void createFeatures();
    qreal calculateSurface(Atom *inAtom, double inRadius, QList<Residue *> inResidues);
    bool AtomIntersectsBox(Atom *atom, qreal inRadius);
private:
    QString m_name;
    QString m_compeleteName;
    char chirality;
    int m_serial;
//    QColor m_color;
    bool m_selected;
    QList<Atom *> m_atoms;
    Vector3D m_Box[2];
    Vector3D m_Center;
    qreal m_Radius;
    qreal m_accessibility;
    bool m_isAminoAcid;
    HBond mHBondDonor[2], mHBondAcceptor[2];
    Chain *m_parent;

    QList<Residue *> neighbours;
    Atom *m_N;
    Atom *m_O;
    Atom *m_CA;
    Atom *m_C;
    Atom *m_H;

    Residue *m_pre;
    Residue *m_next;

    void assignHydrogen();


};

class Accumulator
{
  public:

    struct candidate
    {
        Vector3D	location;
        double	radius;
        double	distance;

        bool operator<(const candidate& rhs) const
                { return distance < rhs.distance; }
    };

    void operator()( Vector3D& a,  Vector3D& b, qreal d, qreal r)
    {
        double distance = pow(a.distanceToPoint(b), 2);
        d += kRadiusWater;
        r += kRadiusWater;

        double test = d + r;
        test *= test;

        if (distance < test && distance > 0.0001)
        {
            candidate c = { b - a, r * r, distance };

            m_x.push_back(c);
            push_heap(m_x.begin(), m_x.end());

        }
    }

    void sort()
    {
        sort_heap(m_x.begin(), m_x.end());
//        qSort(m_x.begin(),m_x.end());

    }

    QList<candidate> m_x;
};

class MSurfaceDots
{
  public:
    static MSurfaceDots&	Instance();

    quint32					size() const					{ return mPoints.size(); }
    const Vector3D&		operator[](quint32 inIx) const	{ return mPoints[inIx]; }
    qreal					weight() const					{ return mWeight; }

  private:
                            MSurfaceDots(qint32 inN);

    vector<Vector3D>		mPoints;
    qreal					mWeight;
};

#endif // AMINOACID_H
