#ifndef CHAIN_H
#define CHAIN_H

#include <QObject>
#include <QMutex>
#include "model.h"
#include "residue.h"
class Chain : public QObject
{
    Q_OBJECT
public:
    explicit Chain(QString id, QObject *parent = 0);
    Chain(Chain *chain);
    ~Chain();
    QList<Residue *> residues();
    QString id(){return m_id;}
    void addResidue(Residue *residue);
    bool operator==(Chain value);
    bool operator!=(Chain value);
    QString operator=(Chain value);
    Residue *findResidueBySerial(int serial);
private:
    QMutex mutex;
    QList<Residue*> m_residues;
    QString m_id;
signals:
    
public slots:
    
};

#endif // CHAIN_H
