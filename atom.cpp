#include "atom.h"

Atom::Atom(int serial, QString name, qreal x, qreal y, qreal z)
{
    selected = false;
    m_serial = serial;
    m_name = name;
    setPos(x,y,z);
    makeAtomFeatures();
    m_bonds = new QList<Bond *>();
}

Atom::Atom(Atom *atom)
{
    m_pos = atom->m_pos;
    m_residue = atom->m_residue;
    m_completeName = atom->m_completeName;
    m_name = atom->m_name;
    m_radius = atom->m_radius;
    m_serial = atom->m_serial;
    m_type = atom->m_type;
    m_pos = atom->m_pos;
}

void Atom::makeAtomFeatures()
{

    if(m_name.at(0)=='S')
    {
        m_completeName = "Sulfur";
        m_type = m_name.mid(1);
        //atomColor.setNamedColor("yellow");

        m_radius = 100.0/25.0;
    }
    else if(m_name.at(0)=='P')
    {
        m_completeName = "Phosphor";
        m_type = m_name.mid(1);
//        atomColor.setNamedColor("orange");
        m_radius = 100.0/25.0;
    }

    else if(m_name=="CL")
    {
        m_completeName = "Chlorine";
        m_type = "";
//        atomColor.setNamedColor("green");
        m_radius = 100.0/25.0;
    }
    else if(m_name=="B")
    {
        m_completeName = "Boron";
        m_type = "";
//        atomColor.setNamedColor("brown");
        m_radius = 85.0/25.0;
    }
    else if(m_name=="NA")
    {
        m_completeName = "Sodium";
        m_type = "";
//        atomColor.setNamedColor("lightblue");
        m_radius = 180.0/25.0;
    }
    else if(m_name=="FE")
    {
        m_completeName = "Iron";
        m_type = "";
//        atomColor.setNamedColor("orange");
        m_radius = 140.0/25.0;
    }
    else if(m_name.at(0)=='N')
    {
        m_completeName = "Nitrogen";
        m_type = m_name.mid(1);
//        atomColor.setNamedColor("blue");
        m_radius = 65.0/25.0;
    }
    else if(m_name.at(0)=='O')
    {
        m_completeName = "Oxygen";
        m_type = m_name.mid(1);
//        atomColor.setNamedColor("red");
        m_radius = 60.0/25.0;
    }
    else if(m_name.at(0)=='C')
    {
        m_completeName = "Carbon";
        m_type = parseCarbonType(m_name.mid(1));
//        atomColor.setNamedColor("gray");
        m_radius = 70.0/25.0;
    }
    else if(m_name.at(0)=='H')
    {
        m_completeName = "Hydrogen";
        m_type = m_name.mid(1);
//        atomColor.setNamedColor("white");
        m_radius = 25.0/25.0;
    }
    else
    {
//        qDebug() << m_name;

        m_completeName = m_name;
        m_type = "";
//        atomColor.setRgb(255,20,147);
        m_radius = 1;
    }


}

QString Atom::parseCarbonType(QString fileType)
{
    if(fileType == "A")
        return "Alpha ( A )";
    if(fileType == "B")
        return "Beta ( B )";
    if(fileType == "G")
        return "Gamma ( G )";
    if(fileType == "D")
        return "Delta ( D )";
    return fileType;
}

Vector3D Atom::operator=(Atom *atom)
{
    return atom->m_pos;
}

bool Atom::isInBackbone()
{
    return m_name == "CA" ||
            m_name == "C" ||
            m_name == "N" ||
            m_name == "O";
}



bool Atom::getSelected() const
{
    return selected;
}

void Atom::setSelected(bool value)
{
    selected = value;
}

