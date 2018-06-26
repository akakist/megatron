#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <QFile>
#include <QMessageBox>

double mmPerPixel=0;
int top=10000, bottom=0;
std::vector<std::vector<QPoint> > leftLines;
std::vector<std::vector<QPoint> > lines;
std::vector<std::vector<QPoint> > rightLines;

int topY(const std::vector<QPoint>& l)
{
    int t=100000;
    for(size_t i=0; i<l.size(); i++)
    {
        if(t>l[i].y())t=l[i].y();
    }
    return t;
}
int bottomY(const std::vector<QPoint>& l)
{
    int b=0;
    for(size_t i=0; i<l.size(); i++)
    {
        if(b<l[i].y())b=l[i].y();
    }
    return b;
}
int leftX(const std::vector<QPoint>& l)
{
    int z=100000;
    for(size_t i=0; i<l.size(); i++)
    {
        if(z>l[i].x())z=l[i].x();
    }
    return z;
}
int rightX(const std::vector<QPoint>& l)
{
    int z=0;
    for(size_t i=0; i<l.size(); i++)
    {
        if(z<l[i].x())z=l[i].x();
    }
    return z;
}
void MainWindow::printLine(size_t n, const std::vector<QPoint>& ls, bool isLeft)
{
    QString out;
    QString fn=QString("%1%2.txt").arg(isLeft?"L":"R").arg(n);
    QString ifn=QString("%1%2.jpg").arg(isLeft?"L":"R").arg(n);
    int t=topY(ls);
    int b=bottomY(ls);
    int l=leftX(ls);
    int r=rightX(ls);
    out.append(fn+"\n");
    out.append(QString("topshift %1\n").arg(double(t-top)*mmPerPixel));
    out.append(QString("bottomshift %1\n").arg(double(bottom-b)*mmPerPixel));

    int lastX=-100;
    int idx=0;
    for(size_t i=0; i<ls.size(); i++)
    {
        double x=double(ls[i].x()-l)*mmPerPixel;
        double y=double(ls[i].y()-t)*mmPerPixel;
        double y2=double(ls[i].y()-t)*mmPerPixel+double(t-top)*mmPerPixel;
        int X=x;
        int Y=y;
        int Y2=y2;

        if(X-lastX>10 || i==ls.size()-1)
        {
            QString s=QString("point %1 = %2 x %3 ( %4 )\n")
                      .arg(idx+1)
                      .arg(X)
                      .arg(Y)
                      .arg(Y2);
            out.append(s);
            lastX=X;
            idx++;
        }
    }
    QFile file(fn);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,"Error",tr("Write open failed\n%1").arg(fn));
        return;
    }
    file.setTextModeEnabled(false);
    //if(file.isTextModeEnabled()) throw CommonError("if(file.isTextModeEnabled()) ");

    //std::string buf=out.toStyledString();
    std::string buf=out.toStdString();
    file.write(buf.data(),buf.size());
    file.close();

    QImage im(r-l+50,b-t+50,QImage::Format_RGB32);
    im.fill(Qt::black);
    for(size_t i=0; i<ls.size(); i++)
    {
        im.setPixel(ls[i].x()-l+20,ls[i].y()-t+20,qRgb(255,0,0));
    }
    im.save(ifn);

}

int sortX(const QPoint &a, const QPoint& b)
{
    return a.x()<b.x();
}
void extract_line(std::vector<QPoint> &v, const QPoint& pz,  QImage& im)
{
    v.push_back(pz);
    im.setPixel(pz,0);
    //QPoint p[8];
    //QPoint q[5][5];
    for(int i=-2; i<=2; i++)
    {
        for(int j=-2; j<=2; j++)
        {
            QPoint z(pz.x()+i,pz.y()+j);

            if(im.pixel(z)!=0)
                extract_line(v,z,im);

            //z.setX(pz.x()+i);
            //q[i][j].setY(pz.y()+j);
        }

    }

    /*for(int i=0;i<8;i++)
        p[i]=pz;
    p[0].setX(p[0].x()-1);

    p[1].setX(p[1].x()+1);

    p[2].setY(p[2].y()-1);

    p[3].setY(p[3].y()+1);

    p[4].setX(p[4].x()+1);
    p[4].setY(p[4].y()+1);

    p[5].setX(p[5].x()-1);
    p[5].setY(p[5].y()+1);

    p[6].setX(p[6].x()+1);
    p[6].setY(p[6].y()-1);

    p[7].setX(p[7].x()+1);
    p[7].setY(p[7].y()+1);*/

    /*for(int i=0;i<8;i++)
    {
        if(im.pixel(p[i])!=0)
            extract_line(v,p[i],im);
    }*/
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ///home/u5/Documents/fofan.png
    ///

    QImage im;
    im.load("/home/u5/Documents/fofan.png");

    for(int x=0; x<im.width(); x++)
    {
        for(int y=0; y<im.height(); y++)
        {
            QRgb c=im.pixel(x,y);
            if(qRed(c)/2<qGreen(c) || qRed(c)<10)
            {
                im.setPixel(x,y,0);
            }
        }
    }
    for(int x=0; x<im.width(); x++)
    {
        for(int y=0; y<im.height(); y++)
        {
            if(im.pixel(x,y)!=0)
            {
                if(y<top) top=y;
                if(y>bottom) bottom=y;
            }
        }
    }
    mmPerPixel=double(700)/(bottom-top);

    printf("bottom-top %d\n",bottom-top);
    printf("mmPerPixel %f\n",mmPerPixel);
    for(int x=0; x<im.width(); x++)
    {
        for(int y=0; y<im.height(); y++)
        {
            if(im.pixel(x,y)!=0)
            {
                std::vector<QPoint> line;
                extract_line(line,QPoint(x,y),im);
                sort(line.begin(),line.end(),sortX);
                lines.push_back(line);
                if(line.begin()->y()< line.rbegin()->y())
                    leftLines.push_back(line);
                else
                    rightLines.push_back(line);
            }
        }
    }

    for(size_t i=0; i<leftLines.size(); i++)
    {
        printLine(i,leftLines[i],true);
    }
    for(size_t i=0; i<rightLines.size(); i++)
    {
        printLine(i,rightLines[i],false);
    }

    for(size_t i=0; i<lines.size(); i++)
    {
        QRgb c=qRgb(255,255,0);
        for(size_t j=0; j<lines[i].size(); j++)
        {
            im.setPixel(lines[i][j],c);
        }
    }


    for(int x=0; x<im.width(); x++)
    {
        im.setPixel(x,top,qRgb(0,255,0));
        im.setPixel(x,bottom,qRgb(0,0,255));
    }

    QPixmap px=QPixmap::fromImage(im);

    ui->label->setPixmap(px);
}

MainWindow::~MainWindow()
{
    delete ui;
}
