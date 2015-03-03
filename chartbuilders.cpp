#include "chartbuilders.h"
#include <QPainter>
#include <QDebug>
#include <QFontDatabase>
#include <iostream>


ChartBuilder::ChartBuilder()
{
    lastMax = 0;
    colorIndex =0;
    m_img = new QImage(PIC_WIDTH,PIC_HEIGHT,QImage::Format_ARGB32);
    m_img->setDotsPerMeterX(10000);
    m_img->setDotsPerMeterY(10000);
    maxDataCount = 0;
    colors << "Red"<<"Blue"<< "Green"<< "purple"<< "darkblue";

}


void ChartBuilder::addData(QString name, QList<int> data)
{
    Data toAdd;
    int max = findMax(data);
    if(max>lastMax)
        lastMax = max;
    toAdd.name = name;
    toAdd.data = data;
    toAdd.color = nextColor();
    datas.append(toAdd);
    if(maxDataCount<data.count())
        maxDataCount = data.count();
}

void ChartBuilder::saveImage(QString fileName)
{

    QPainter painter(m_img);

    painter.setViewport(X_OFFSET, Y_OFFSET,PIC_WIDTH - 4*X_OFFSET, PIC_HEIGHT - 4*Y_OFFSET);

    painter.setBackground(QBrush(QColor("white")));
    QPen pen;
    pen.setWidth(3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    painter.save();
    drawChartLines(&painter);
    painter.restore();
    writeLabels(&painter);

    painter.scale(1,-1);
    painter.translate(30,-painter.viewport().height());

    double sx = painter.viewport().width()*1.0/maxDataCount;
    double sy = painter.viewport().height()*1.0/lastMax;
    // painter.scale(sx, sy);


    QPainterPath *path;
    for(int i=0;i<datas.count();i++)
    {
        path = new QPainterPath();
        for(int j=0;j<datas[i].data.count();j++)
        {
            if(j!=0)
                path->lineTo(j*sx, datas[i].data[j]*sy);
            path->moveTo(j*sx, datas[i].data[j]*sy);
            //            painter.drawPoint(j*sx, datas[i][j]*sy);
        }
        pen.setColor(datas[i].color);
        painter.setPen(pen);
        painter.drawPath(*path);

        delete path;
    }

   if(!m_img->save(fileName, "PNG", 100))
        std::cout << "could not save image on " << fileName.toStdString() << std::endl;
}

void ChartBuilder::setLabels(QStringList labels)
{
    m_labels = labels;
}


int ChartBuilder::findMax(QList<int> data)
{
    int max = 0;

    for(int j=0;j<data.count();j++)
    {
        if(max < data[j])
            max = data[j];
    }
    return max;
}

QColor ChartBuilder::nextColor()
{

    if(colorIndex>=5)
        return QColor(rand()%256,rand()%256,rand()%256);
    return QColor(colors[colorIndex++]);

}

void ChartBuilder::writeLabels(QPainter *pdc)
{
    pdc->save();
    int x0 = pdc->viewport().width()-30;
    int y0 = pdc->viewport().y();
    QRectF rec;
    pdc->setFont(QFont("arial",4));
    rec = pdc->boundingRect(rec,"2014,TarbiatModaresUnversity,R.Hajizadeh.");
    pdc->drawText(PIC_WIDTH-rec.width()-40,PIC_HEIGHT-rec.height()+25
                  ,rec.width()+40,rec.height(),0,"2014, Tarbiat Modares Unversity, R.Hajizadeh.");
    pdc->setViewport(x0,y0,PIC_HEIGHT,PIC_HEIGHT);
    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pdc->setPen(pen);


    for(int i=0;i<datas.count();i++)
    {
        pdc->setBrush(QBrush(datas[i].color));
        double a = 40;
        double y = i*(a+5)+5;

        pdc->drawRoundedRect(0,y,a,a-10,10,10);


        /*rec = pdc->boundingRect(rec,datas[i].name);
        pdc->drawText(a+5,y+a/2 -rec.height()/2.0-5,rec.width(),rec.height(),0,datas[i].name);*/
    }
    pdc->restore();
}
QString ChartBuilder::verticalLabel() const
{
    return m_verticalLabel;
}

void ChartBuilder::setVerticalLabel(const QString &verticalLabel)
{
    m_verticalLabel = verticalLabel;
}

QString ChartBuilder::horzontalLabel() const
{
    return m_horzontalLabel;
}

void ChartBuilder::setHorzontalLabel(const QString &horzontalLabel)
{
    m_horzontalLabel = horzontalLabel;
}


void ChartBuilder::drawChartLines(QPainter *pdc)
{

    pdc->save();
    int xend=pdc->viewport().width();
    int y0 = pdc->viewport().height();
    pdc->setFont(QFont("arial",5));
    /*QRectF rec;
    pdc->translate(30,0);
    pdc->drawLine(0, y0, xend, y0);
    rec = pdc->boundingRect(rec,m_verticalLabel);
    pdc->drawText((xend-rec.width())/2.0, y0+60,rec.width()+25,rec.height(),0,m_verticalLabel);*/

    pdc->drawLine(0, y0, 0, 0);

    //rec = pdc->boundingRect(rec,m_horzontalLabel);

    pdc->save();
    //pdc->translate(-rec.width()-30, y0/2.0);
    pdc->rotate(-90);
    //pdc->drawText(0, 0,rec.width(),rec.height(),0,m_horzontalLabel);
    pdc->restore();

    qreal sx =  pdc->viewport().width()*1.0/maxDataCount;
    qreal sy =   pdc->viewport().height()*1.0/lastMax;
    QString s;

    QPen pen = pdc->pen();
    int lineWidth = pen.width();


    qreal space = 10;
    QVector<qreal> dashes;
     dashes << 3 << space;
  //draw x vector
     int step = 1;
     if(maxDataCount>50)
         step=3;
     if(maxDataCount>100)
         step=6;
     if(maxDataCount>300)
         step=10;
     if(maxDataCount>400)
         step=20;
     if(maxDataCount>900)
         step=20;
    for(int i=0;i<maxDataCount;i+=step)
    {
        pdc->drawLine(i*sx, y0,i*sx,y0+10);

        pen.setDashPattern(dashes);
        pen.setWidth(lineWidth-2.1);
        pdc->setPen(pen);
        pdc->drawLine(i*sx, y0,i*sx,0);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(lineWidth);
        pdc->setPen(pen);


        s = QString::number(i);

        if(i<m_labels.count())
           s = m_labels[i];

        pdc->save();
        pdc->translate(i*sx,y0+13);
        pdc->rotate(90);
       // rec = pdc->boundingRect(rec,s);
      //  pdc->drawText(0,-rec.width()/2.0,rec.width()+30,rec.height(),Qt::TextSingleLine,s);
        pdc->restore();

    }
    step = 0;
    if(lastMax<100)
        step = 10;
    else if(lastMax<2000)
        step = 100;
    else if(lastMax<5000)
        step = 200;
    else if(lastMax<8000)
        step = 500;
    else if(lastMax<1000)
        step = 1000;
    else
        step = 1500;
    //draw y vector
    for(int i=0;i<lastMax;i+=step)
    {
        pdc->drawLine(0, y0-i*sy,-10,y0-i*sy);
        pen.setDashPattern(dashes);
        pen.setWidth(lineWidth-2.1);
        pdc->setPen(pen);
        pdc->drawLine(0, y0-i*sy,xend,y0-i*sy);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(lineWidth);
        pdc->setPen(pen);
        s = QString::number(i);

      //  rec = pdc->boundingRect(rec,s);
       // pdc->drawText(-rec.width()-10, y0-i*sy-rec.height()/2.0,rec.width(),rec.height(),Qt::TextSingleLine,s);
    }
    pdc->restore();
}
