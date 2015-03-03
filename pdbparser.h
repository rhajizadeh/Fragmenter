#ifndef PDBPARSER_H
#define PDBPARSER_H

#include <QThread>
#include <QFile>
#include "chain.h"
#include "atom.h"
#include "model.h"
class PdbParser : public QThread
{
    Q_OBJECT
public:
    explicit PdbParser(QObject *parent=0);
     QList<Model *> models(){return modelList;}
     QString title() { return m_title; }
     QString classification() {return m_classification;}
     void setFileName(QString fileName);

     QStringList atomsIncluded(){return m_atomsIncluded;}
     QStringList atomsInBackboneIncluded(){return m_backboneAtomsIncluded;}
     QStringList residuesIncluded(){return m_residuesIncluded;}

     int getaAtomCount(QString name);
     int getaAminoCount(QString name);

     QList<int> getAtomCount(){return m_atomCount;}
     QList<int> getInBackboneAtomCount(){return m_atomsInBackboneCount;}
     QList<int> getResidueCount(){return m_residueCount;}
    bool isSuccessful(){return !error;}

protected:
    int parseModel(QString line);
    Atom *parseAtom(QString line, Model *model);
    void parseBonds(Atom *atom, Model *model);
    void run();
    void clearLists();
    bool parseResidue(QString line, Model *model);
    void countAtom(Atom *atom);
    void countResidue(Residue *residue);

private:
    QFile *file;
    QString fileName;
    QString m_title;
    QString m_classification;
    int currModel;
    QList<Residue*> m_residues;

    Residue *currentResidue;
    Chain *currChain;
    QList<Model *> modelList;
    QString message;
    QStringList m_atomsIncluded;
    QStringList m_backboneAtomsIncluded;
    QStringList m_residuesIncluded;

    QList<int> m_atomCount;
    QList<int> m_atomsInBackboneCount;
    QList<int> m_residueCount;
    bool error;

signals:
    void readProgress(int n,int total);
    void parsingDone();
    void parseError(QString message);
    void newAtomSig(Atom *atom);
public slots:
    
};

#endif // PDBPARSER_H
