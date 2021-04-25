#include <vector>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>

#include "Window.h"

using namespace std;

class Polynome
{
public:
    Polynome(vector<double> coeff);
    double eval(double point);
    void operator*=(Polynome const& b);
    void operator*=(double a);
    void operator+=(Polynome const& b);
    Polynome deriver();
    void plot(Window *win,double pas=1);
    static Polynome interpolateur(vector<double> points,int no);
    static Polynome interpolation(vector<double> x,vector<double> y);
    int getSize(){return m_coeff.size();};
private:
    vector<double> m_coeff;
};

Polynome::Polynome(vector<double> coeff)
:m_coeff(coeff)
{}

double Polynome::eval(double point)
{
    double somme=0;
    for(int i=m_coeff.size()-1;i>=0;i--)
    {
        somme=somme*point+m_coeff[i];
    }
    return somme;
}

void Polynome::operator*=(Polynome const& b)
{
    vector<double> resultat;
    for(int i=0;i<b.m_coeff.size()+m_coeff.size()-1;i++)
    {
        double somme=0;
        for(int k=0;k<=i;k++)
        {
            if(k<b.m_coeff.size()&&(i-k)<m_coeff.size())
            {
                somme+=b.m_coeff[k]*m_coeff[i-k];
            }
        }
        resultat.push_back(somme);
    }
    m_coeff=resultat;
}

Polynome operator*(Polynome const& a,Polynome const& b)
{
    Polynome resultat(a);
    resultat*=b;

    return resultat;
}


void Polynome::operator*=(double a)
{
    for(int i=0;i<m_coeff.size();i++)
    {
        m_coeff[i]*=a;
    }
}

Polynome operator*(double const& a,Polynome const& b)
{
    Polynome resultat(b);
    resultat*=a;

    return resultat;
}

void Polynome::operator+=(Polynome const& b)
{
    vector<double> resultat;
    for(int i=0;i<max(m_coeff.size(),b.m_coeff.size());i++)
    {
        resultat.push_back(0);
        if(i<m_coeff.size())
            resultat[i]+=m_coeff[i];
        if(i<b.m_coeff.size())
            resultat[i]+=b.m_coeff[i];
    }

    m_coeff=resultat;
}

Polynome operator+(Polynome const& a,Polynome const& b)
{
    Polynome resultat(a);
    resultat+=b;

    return resultat;
}

Polynome Polynome::deriver()
{
    vector<double> resultat;
    for(int i=0;i<m_coeff.size()-1;i++)
    {
        resultat.push_back((i+1)*m_coeff[i+1]);
    }

    return Polynome(resultat);
}

void Polynome::plot(Window *win,double pas)
{
    double x(0);
    double a;
    Polynome der(deriver());
    while(x<win->getwidth())
    {
        while((!(win->setPixel(x,win->getheight()-eval(x),Color(0,0,0))))&&x<win->getwidth())
        {
            x+=pas;
        }
        a=der.eval(x);
        x+=1./sqrt(1+a*a);
    }
}

Polynome Polynome::interpolateur(vector<double> points,int no)
{
    Polynome poly({1});
    for(int i=0;i<points.size();i++)
    {
        if(i!=no)
        {
            poly*=(1./(points[no]-points[i]))*Polynome({-points[i],1});
        }
    }
    return poly;
}

Polynome Polynome::interpolation(vector<double> x,vector<double> y)
{
    Polynome resultat({0});
    for(int i=0;i<x.size();i++)
    {
        resultat+=y[i]*Polynome::interpolateur(x,i);
    }
    return resultat;
}

int main( int argc, char * argv[] )
{
    Window win(500,500,"lagrange");

    Polynome poly({250});
    vector<double> x;
    vector<double> y;

    Coord coord;
    while(true)
    {
        win.clean(Color(255,255,255));
        poly.plot(&win,0.0001);
        win.update();
        win.save("test.bmp");
        coord=win.startLoop();
        if(coord.x==-1)
            break;
        x.push_back(coord.x);
        y.push_back(win.getheight()-coord.y);
        poly=Polynome::interpolation(x,y);
    }

    return 0;
}
