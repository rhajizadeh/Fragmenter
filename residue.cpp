#include "residue.h"
#include <qmath.h>
#include <limits>
#include <QDebug>
double CosinusAngle(const Vector3D p1, const Vector3D p2, const Vector3D p3, const Vector3D p4)
{
    Vector3D v12 = p1 - p2;
    Vector3D v34 = p3 - p4;

    double result = 0;

    double x = Vector3D::dotProduct(v12, v12) * Vector3D::dotProduct(v34, v34);
    if (x > 0)
        result = Vector3D::dotProduct(v12, v34) / sqrt(x);

    return result;
}

double DihedralAngle(const Vector3D p1, const Vector3D p2, const Vector3D p3, const Vector3D p4)
{
    Vector3D v12 = p1 - p2;	// vector from p2 to p1
    Vector3D v43 = p4 - p3;	// vector from p3 to p4

    Vector3D z = p2 - p3;		// vector from p3 to p2

    Vector3D p = Vector3D::crossProduct(z, v12);
    Vector3D x = Vector3D::crossProduct(z, v43);
    Vector3D y = Vector3D::crossProduct(z, x);

    double u = Vector3D::dotProduct(x, x);
    double v = Vector3D::dotProduct(y, y);

    double result = 360;
    if (u > 0 && v > 0)
    {
        u = Vector3D::dotProduct(p, x) / sqrt(u);
        v = Vector3D::dotProduct(p, y) / sqrt(v);
        if (u != 0 || v != 0)
            result = atan2(v, u) * 180 / M_PI;
    }

    return result;
}


Residue::Residue(int serial, QString name,Chain *parentChain, Residue *pre) :
    m_parent(parentChain),m_pre(pre)
{
    m_serial = serial;
    m_name = name ;
    m_selected = false;
    m_N = m_C = m_CA = m_O = m_H = 0;
    m_accessibility = 0;

    m_pre = 0;
    m_next = 0;
    createFeatures();

    if(m_pre!=0)
        m_pre->m_next = this;

    mHBondAcceptor[0].residue = 0;
    mHBondAcceptor[1].residue = 0;
    mHBondDonor[0].residue = 0;
    mHBondDonor[1].residue = 0;

    mHBondDonor[0].energy
            = mHBondDonor[1].energy
            = mHBondAcceptor[0].energy
            = mHBondAcceptor[1].energy = 0;

    m_accessibility = 0;

    m_Box[0].setX( std::numeric_limits<float>::max());
    m_Box[0].setY( std::numeric_limits<float>::max());
    m_Box[0].setZ( std::numeric_limits<float>::max());

    m_Box[1].setX(-std::numeric_limits<float>::max());
    m_Box[1].setY(-std::numeric_limits<float>::max());
    m_Box[1].setZ(-std::numeric_limits<float>::max());
}

Residue::Residue(Residue *residue)
{
    m_serial = residue->m_serial;
    m_name = residue->m_name;
    m_compeleteName = residue->m_compeleteName;
    m_parent = residue->m_parent;
    m_pre = residue->m_pre;
    m_next = residue->m_next;
    m_accessibility=0;

    m_N = residue->m_N;
    m_C = residue->m_C;
    m_CA = residue->m_CA;
    m_O = residue->m_O;
    m_H = residue->m_H;

    chirality = residue->chirality;

    m_Center = residue->m_Center;
    m_Radius = residue->m_Radius;
    m_isAminoAcid = residue->m_isAminoAcid;

    m_Box[0] = residue->m_Box[0];
    m_Box[1] = residue->m_Box[1];
    mHBondDonor[0] = residue->mHBondDonor[0];
    mHBondDonor[1] = residue->mHBondDonor[1];

    mHBondAcceptor[0] = residue->mHBondAcceptor[0];
    mHBondAcceptor[1] = residue->mHBondAcceptor[1];


    for(int i=0;i<residue->m_atoms.count();i++)
    {
        m_atoms.append(new Atom(residue->m_atoms[i]));
    }


}

Residue::~Residue()
{
    Atom *atom;
    while(m_atoms.count()>0)
    {
        atom = m_atoms.takeFirst();
        delete atom;
    }
}


void Residue::createFeatures()
{
    m_isAminoAcid = true;

    if(m_name == "ALA" )
    {
        m_compeleteName = "Alanine";
//        m_color.setNamedColor("lightgreen");
    }
    else if(m_name == "ARG" )
    {
        m_compeleteName = "Arginine";
//        m_color.setNamedColor("blue");
    }
    else if(m_name == "ASN")
    {
        m_compeleteName = "Asparagine";
//        m_color.setNamedColor("darkgreen");
    }
    else if(m_name == "ASP")
    {
        m_compeleteName = "Aspartic acid";
//        m_color.setNamedColor("darkgreen");
    }
    else if(m_name == "ASX"  )
    {
        m_compeleteName = "Asparagine or Aspartic acid";
//        m_color.setNamedColor("gray");
    }
    else if(m_name == "CYS"  )
    {
        m_compeleteName = "Cysteine";
//        m_color.setNamedColor("green");
    }
    else if(m_name == "GLN")
    {
        m_compeleteName = "Glutamine";
//        m_color.setNamedColor("darkgreen");
    }
    else if(m_name == "GLU" )
    {
        m_compeleteName = "Glutamic acid";
//        m_color.setNamedColor("drakgreen");
    }
    else if(m_name == "GLX" )
    {
        m_compeleteName = "Glutamine or Glutamic acid";
//        m_color.setNamedColor("lightgray");
    }
    else if(m_name == "GLY" )
    {
        m_compeleteName = "Glycine";
//        m_color.setNamedColor("lightgreen");
    }
    else if(m_name == "HIS" )
    {
        m_compeleteName = "Histidine";
//        m_color.setNamedColor("darkblue");
    }
    else if(m_name == "ILE" )
    {
        m_compeleteName = "Isoleucine";
//        m_color.setNamedColor("blue");
    }
    else if(m_name == "LEU")
    {
        m_compeleteName = "Leucine";
//        m_color.setNamedColor("blue");
    }
    else if(m_name == "LYS" )
    {
        m_compeleteName = "Lysine";
//        m_color.setNamedColor("range");
    }
    else if(m_name == "MET")
    {
        m_compeleteName = "Methionine";
//        m_color.setNamedColor("blue");
    }
    else if(m_name == "PHE" )
    {
        m_compeleteName = "Phenylalanine";
//        m_color.setNamedColor("lilac");
    }
    else if(m_name == "PRO")
    {
        m_compeleteName = "Proline";
//        m_color.setNamedColor("pink");
    }
    else if(m_name == "SER" )
    {
        m_compeleteName = "Serine";
//        m_color.setNamedColor("red");
    }
    else if(m_name == "THR")
    {
        m_compeleteName = "Threonine";
//        m_color.setNamedColor("red");
    }
    else if(m_name == "TRP" )
    {
        m_compeleteName = "Tryptophan";
//        m_color.setNamedColor("lilac");
    }
    else if(m_name == "TYR")
    {
        m_compeleteName = "Tyrosine";
//        m_color.setNamedColor("lilac");
    }
    else if(m_name == "VAL")
    {
        m_compeleteName = "Valine";
//        m_color.setNamedColor("blue");
    }
    else if(m_name == "A")
    {
        m_compeleteName = "Adenosine";
        m_isAminoAcid = false;
    }
    else if(m_name == "C")
    {
        m_compeleteName = "Cytidine";
//        m_isAminoAcid = false;
    }
    else if(m_name == "G")
    {
        m_compeleteName = "Guanosine";
//        m_isAminoAcid = false;
    }
    else if(m_name == "I")
    {
        m_compeleteName = "Inosine";
        m_isAminoAcid = false;
    }
    else if(m_name == "T")
    {
        m_compeleteName = "Thymidine";
        m_isAminoAcid = false;
    }
    else if(m_name == "U")
    {
        m_compeleteName = "Uridine";
        m_isAminoAcid = false;
    }
    else if(m_name == "UNK")
    {
        m_compeleteName = "Unknown";
        m_isAminoAcid = false;
    }
    else
    {
        m_compeleteName = m_name;
        m_isAminoAcid = false;
    }
//    m_color.setRgb(qrand()%255,qrand()%255,qrand()%255);
}

void Residue::calculateSurface(QList<Residue*> inResidues)
{
    QList<Residue *> neighbours ;
    double d;
    QList<Residue*>::Iterator it;
    for(it=inResidues.begin(); it<inResidues.end(); it++)
    {
        d = m_Center.distanceToPoint((*it)->m_Center);
        if(d<m_Radius + (*it)->m_Radius)
        {
            neighbours.append(*it);
        }
    }

    m_accessibility =
            calculateSurface(m_N, kRadiusN, neighbours) +
            calculateSurface(m_CA, kRadiusCA, neighbours) +
            calculateSurface(m_C, kRadiusC, neighbours) +
            calculateSurface(m_O, kRadiusO, neighbours);
    foreach ( Atom *atom, m_atoms)
    {
        m_accessibility += calculateSurface(atom, kRadiusSideAtom, neighbours);

    }
}

double Residue::Kappa() const
{
    double result = 360;
    const Residue* prevPrev = m_pre ? m_pre->m_pre : 0;
    const Residue* nextNext = m_next ? m_next->m_next : 0;
    if (prevPrev != 0 && nextNext != 0 /*and NoChainBreak(prevPrev, nextNext)*/)
    {
        double ckap = CosinusAngle(m_CA->center(),
                                   prevPrev->m_CA->center(),
                                   nextNext->m_CA->center(),
                                   m_CA->center());
        double skap = sqrt(1 - ckap * ckap);
        result = atan2(skap, ckap) * 180 / M_PI;
    }
    return result;
}

double Residue::TCO() const
{
    double result = 0;
    if (m_pre != 0 /*and NoChainBreak(mPrev, this)*/)
        result = CosinusAngle(m_C->center(), m_O->center(), m_pre->m_C->center(), m_pre->m_O->center());
    return result;
}

double Residue::Alpha()
{
    double alhpa = 360;
    chirality = ' ';

    const Residue* nextNext = m_next ? m_next->m_next : 0;
    if (m_pre != 0 && nextNext != 0 /*and NoChainBreak(mPrev, nextNext)*/)
    {
        alhpa = DihedralAngle(m_pre->m_CA->center(), m_CA->center(), m_next->m_CA->center(), nextNext->m_CA->center());
        if (alhpa < 0)
            chirality = '-';
        else
            chirality = '+';
    }
    return alhpa;
}

double Residue::Phi() const
{
    double result = 360;
    if (m_pre != 0 /*and NoChainBreak(mPrev, this)*/)
        result = DihedralAngle(m_pre->m_C->center(), m_N->center(), m_CA->center(), m_C->center());
    return result;
}

double Residue::Psi() const
{
    double result = 360;
    if (m_next != 0 /*and NoChainBreak(this, mNext)*/)
        result = DihedralAngle(m_N->center(), m_CA->center(), m_C->center(), m_next->m_N->center());
    return result;
}

double Residue::calculateHBondEnergy(Residue *inDonor ,Residue *inAcceptor)
{
    double result = 0;
    if (inDonor->m_name != "Proline")
    {
        double distanceHO = inDonor->getH()->center().distanceToPoint(inAcceptor->getO()->center());
        double distanceHC = inDonor->getH()->center().distanceToPoint(inAcceptor->getC()->center());
        double distanceNC = inDonor->getN()->center().distanceToPoint(inAcceptor->getC()->center());
        double distanceNO = inDonor->getN()->center().distanceToPoint(inAcceptor->getO()->center());

        if (distanceHO < kMinimalDistance ||
                distanceHC < kMinimalDistance ||
                distanceNC < kMinimalDistance ||
                distanceNO < kMinimalDistance)
            result = kMinHBondEnergy;
        else
            result = kCouplingConstant / distanceHO
                    - kCouplingConstant / distanceHC
                    + kCouplingConstant / distanceNC
                    - kCouplingConstant / distanceNO;

        // DSSP compatibility mode:
        result = qRound(result * 1000) / 1000.0;

        if (result < kMinHBondEnergy)
            result = kMinHBondEnergy;
    }


    // update donor
    if (result < inDonor->mHBondAcceptor[0].energy)
    {
        inDonor->mHBondAcceptor[1] = inDonor->mHBondAcceptor[0];
        inDonor->mHBondAcceptor[0].residue = inAcceptor;
        inDonor->mHBondAcceptor[0].energy = result;
    }
    else if (result < inDonor->mHBondAcceptor[1].energy)
    {
        inDonor->mHBondAcceptor[1].residue = inAcceptor;
        inDonor->mHBondAcceptor[1].energy = result;
    }

    // and acceptor
    if (result < inAcceptor->mHBondDonor[0].energy)
    {
        inAcceptor->mHBondDonor[1] = inAcceptor->mHBondDonor[0];
        inAcceptor->mHBondDonor[0].residue = inDonor;
        inAcceptor->mHBondDonor[0].energy = result;
    }
    else if (result < inAcceptor->mHBondDonor[1].energy)
    {
        inAcceptor->mHBondDonor[1].residue = inDonor;
        inAcceptor->mHBondDonor[1].energy = result;
    }

    return result;
}

qreal Residue::calculateSurface(Atom *inAtom, double inRadius, QList<Residue *> inResidues)
{
    Accumulator accumulate;

    foreach (Residue *r, inResidues)
    {
        if (r->AtomIntersectsBox(inAtom, inRadius))
        {
            accumulate(inAtom->center(), r->m_N->center(), inRadius, kRadiusN);
            accumulate(inAtom->center(), r->m_CA->center(), inRadius, kRadiusCA);
            accumulate(inAtom->center(), r->m_C->center(), inRadius, kRadiusC);
            accumulate(inAtom->center(), r->m_O->center(), inRadius, kRadiusO);

            foreach (Atom *atom, r->m_atoms)
                accumulate(inAtom->center(), atom->center(), inRadius, kRadiusSideAtom);
        }
    }

    accumulate.sort();

    double radius = inRadius + kRadiusWater;
    double surface = 0;

    MSurfaceDots& surfaceDots = MSurfaceDots::Instance();

    for (quint32 i = 0; i < surfaceDots.size(); i++)
    {
        Vector3D xx = surfaceDots[i] * radius;

        bool free = true;
        for (quint32 k = 0; free && k < accumulate.m_x.size(); k++)
        {
            free = accumulate.m_x[k].radius
                    < pow(xx.distanceToPoint(accumulate.m_x[k].location),2);
        }
        if (free)
            surface += surfaceDots.weight();
    }
    return surface * radius * radius;
}

qreal Residue::accessibility() const
{
    return m_accessibility;
}

void Residue::resetAccessibility()
{
    m_accessibility = 0;
}

void Residue::assignHydrogen()
{
    m_H = new Atom(*m_N);

    if (m_compeleteName != "Proline" && m_pre != 0)
    {
        Atom *pc = m_pre->getC();
        Atom *po = m_pre->getO();
        if(po == 0 || pc == 0)
            return;
        double CODistance = pc->center().distanceToPoint(po->center());

        m_H->setPos(m_H->x() + (pc->x() - po->y())/CODistance,
                    m_H->y() + (pc->y() - po->y())/CODistance,
                    m_H->z() + (pc->z() - po->z())/CODistance);

    }

}
bool Residue::selected() const
{
    return m_selected;
}

void Residue::setSelected(bool selected)
{
    m_selected = selected;
}

void Residue::addAtom(Atom *atom)
{
    if(atom->name()=="CA")
    {
        m_CA = atom;

    }
    else if(atom->name()=="C")
    {
        m_C = atom;
    }
    else if(atom->name()=="N")
    {
        m_N = atom;
        assignHydrogen();
    }
    else if(atom->name()=="O")
    {
        m_O = atom;
    }
    else if(atom->name().at(0)!='H')
    {
        m_atoms.append(atom);
    }


    atom->setAminoAcid(this);
}

bool Residue::finalizeResidue(QString &message)
{
    if(!isAminoAcid())
        return true;
    m_Box[0].setX( std::numeric_limits<float>::max());
    m_Box[0].setY( std::numeric_limits<float>::max());
    m_Box[0].setZ( std::numeric_limits<float>::max());

    m_Box[1].setX(-std::numeric_limits<float>::max());
    m_Box[1].setY(-std::numeric_limits<float>::max());
    m_Box[1].setZ(-std::numeric_limits<float>::max());

    message = "Residue %1 in chain %2 doesnt have %3 atom\n";
    if(m_N == 0)
    {
        message = message.arg(QString::number(serial()),parentChain()->id(),"Nitrogen");
        return false;
    }
    if(m_CA == 0)
    {
        message = message.arg(QString::number(serial()),parentChain()->id(),"Carbon Alpha ");
        return false;
    }
    if(m_C == 0)
    {
        message = message.arg(QString::number(serial()),parentChain()->id(),"Carbon");
        return false;
    }
    if(m_O == 0)
    {
        message = message.arg(QString::number(serial()),parentChain()->id(),"Oxygen");
        return false;
    }
    extendBox(m_N, kRadiusN + 2 * kRadiusWater);
    extendBox(m_CA, kRadiusCA + 2 * kRadiusWater);
    extendBox(m_C, kRadiusC + 2 * kRadiusWater);
    extendBox(m_O, kRadiusO + 2 * kRadiusWater);
    foreach (Atom *atom, m_atoms)
        extendBox(atom, kRadiusSideAtom + 2 * kRadiusWater);


    m_Radius = m_Box[1].x() - m_Box[0].x();
    if (m_Radius < m_Box[1].y() - m_Box[0].y())
        m_Radius = m_Box[1].y() - m_Box[0].y();
    if (m_Radius < m_Box[1].z() - m_Box[0].z())
        m_Radius = m_Box[1].z() - m_Box[0].z();

    m_Center.setX((m_Box[0].x() + m_Box[1].x()) / 2.0);
    m_Center.setY((m_Box[0].y() + m_Box[1].y()) / 2.0);
    m_Center.setZ((m_Box[0].z() + m_Box[1].z()) / 2.0);
    return true;

}

void Residue::addNeighbours(Residue *neighbour)
{
    neighbours.append(neighbour);
}



QList<Atom *> Residue::getAllAtoms()
{
    QList<Atom *> al ;
    if(m_N!=0)
        al.append(m_N);
    if(m_O!=0)
        al.append(m_O);
    if(m_H!=0)
        al.append(m_H);
    if(m_CA!=0)
        al.append(m_CA);
    if(m_C!=0)
        al.append(m_C);
    al.append(m_atoms);
    return al;
}

QStringList Residue::getAllAtomsNames()
{
    QStringList list;
    QList<Atom *> atoms = getAllAtoms();
    for(int i=0; i<atoms.count(); i++)
    {
        list.append(atoms.at(i)->name() + " "
                    + QString::number(atoms.at(i)->x()) + " "
                    + QString::number(atoms.at(i)->y()) + " "
                    + QString::number(atoms.at(i)->z()));

    }
    return list;
}

QList<Atom *> Residue::getBackboneAtoms()
{
    QList<Atom *> al;
    if(m_N!=0)
        al.append(m_N);
    if(m_O!=0)
        al.append(m_O);
    if(m_H!=0)
        al.append(m_H);
    if(m_CA!=0)
        al.append(m_CA);
    if(m_C!=0)
        al.append(m_C);
    return al;
}

inline
bool Residue::AtomIntersectsBox(Atom *atom, double inRadius)
{
    return
            (atom->x() + inRadius >= m_Box[0].x()) && (atom->x() - inRadius <= m_Box[1].x()) &&
            (atom->y() + inRadius >= m_Box[0].y()) && (atom->y() - inRadius <= m_Box[1].y()) &&
            (atom->z() + inRadius >= m_Box[0].z()) && (atom->z() - inRadius <= m_Box[1].z());
}
bool Residue::isAminoAcid() const
{
    return m_isAminoAcid;
}

void Residue::setIsAminoAcid(bool isAminoAcid)
{
    m_isAminoAcid = isAminoAcid;
}


void Residue::extendBox(Atom *atom, int inRadius)
{
    if (m_Box[0].x() >( atom->x() - inRadius))
        m_Box[0].setX(atom->x() - inRadius);
    if (m_Box[0].y() > (atom->y() - inRadius))
        m_Box[0].setY(atom->y() - inRadius);
    if (m_Box[0].z() > (atom->z() - inRadius))
        m_Box[0].setZ(atom->z() - inRadius);

    if (m_Box[1].x() < (atom->x() + inRadius))
        m_Box[1].setX(atom->x() + inRadius);
    if (m_Box[1].y() < (atom->y() + inRadius))
        m_Box[1].setY(atom->y() + inRadius);
    if (m_Box[1].z() < (atom->z() + inRadius))
        m_Box[1].setZ(atom->z() + inRadius);
}

MSurfaceDots& MSurfaceDots::Instance()
{
    const qint32 kN = 200;

    static MSurfaceDots sInstance(kN);
    return sInstance;
}

MSurfaceDots::MSurfaceDots(qint32 N)
{
    qint32 P = 2 * N + 1;

    const double
            kGoldenRatio = (1 + sqrt(5.0)) / 2;
    mWeight = (4 * M_PI) / P;
    for (qint32 i = -N; i <= N; ++i)
    {
        double lat = asin((2.0 * i) / P);
        double lon = fmod(i, kGoldenRatio)* 2 * M_PI / kGoldenRatio;

        Vector3D p;
        p.setX(sin(lon) * cos(lat));
        p.setY(cos(lon) * cos(lat));
        p.setZ(sin(lat));

        mPoints.push_back(p);
    }
}


