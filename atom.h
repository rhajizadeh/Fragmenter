#ifndef ATOM_H
#define ATOM_H

#include <QObject>
#include <QtGui/QVector3D>
#include <QDebug>
#include "residue.h"
class Bond;
class Residue;
class Atom
{
public:
    explicit Atom(int serial,QString name, qreal x,qreal y,qreal z);
    Atom(Atom* atom);
    QVector3D center(){return m_pos;}
    void setPos(QVector3D pos){m_pos = pos;}
    void setPos(qreal x,qreal y,qreal z)
    {m_pos.setX(x); m_pos.setY(y); m_pos.setZ(z);}

    QString name(){return m_name;}
    QString atomName(){return m_completeName;}

    int serial(){return m_serial;}
    qreal x(){return m_pos.x();}
    qreal y(){return m_pos.y();}
    qreal z(){return m_pos.z();}
    QColor color(){return atomColor;}
    qreal radius(){return m_radius;}

    void setAminoAcid(Residue *aminoAcid)
    {m_residue = aminoAcid;}
    Residue *residue(){return m_residue;}

    void addBond(Bond *bond){m_bonds->append(bond);}
    QList<Bond *>* bonds(){return m_bonds;}
    QString getType(){return m_type;}
    bool getSelected() const;
    void setSelected(bool value);

    QString chain() const;
    void setChain(const QString &chain);

    QVector3D operator=(Atom *);
    bool isInBackbone();
protected:
    void makeAtomFeatures();
    QString parseCarbonType(QString fileType);
    //    QList<Bond *>bonds;
private:
    QVector3D m_pos;
    int m_serial;
    QString m_name;
    QString m_completeName;
    QString m_type;
    QColor atomColor;
    qreal m_radius;
    Residue *m_residue;
    QList<Bond *>* m_bonds;

    bool selected;
};

#endif // ATOM_H
