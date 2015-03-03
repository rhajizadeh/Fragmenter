#ifndef CHARTBUILDERS_H
#define CHARTBUILDERS_H

#include <QThread>
#include <QImage>
#include <QStringList>
#include <QColor>

class ChartBuilder
{
    const static int X_OFFSET = 35;
    const static int Y_OFFSET = 25;
    const static int PIC_WIDTH = 1200;
    const static int PIC_HEIGHT = 800;
    struct Data
    {
        QString name;
        QColor color;
        QList<int> data;
    };
public:
    explicit ChartBuilder();
    void addData(QString name, QList<int> data);
    void saveImage(QString fileName);
    void setLabels(QStringList labels);


    QString horzontalLabel() const;
    void setHorzontalLabel(const QString &horzontalLabel);

    QString verticalLabel() const;
    void setVerticalLabel(const QString &verticalLabel);

protected:
    int findMax(QList<int> data);
    int maxDataCount;
    QImage *getImage(){return m_img;}
    QColor nextColor();
    void writeLabels(QPainter *pdc);
private:
    QList<Data >  datas;
    QStringList colors ;
    QImage *m_img;
    int lastMax;
    int colorIndex;
    QStringList m_labels;
    QString m_horzontalLabel, m_verticalLabel;

    void drawChartLines(QPainter *pdc);
};

#endif // CHARTBUILDERS_H
