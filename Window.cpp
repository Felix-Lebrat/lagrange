#include "Window.h"
#include <iostream>

using namespace std;

Coord operator+(Coord const& a,Coord const& b)
{
    return Coord(a.x+b.x,a.y+b.y);
}

Coord operator*(double const& a,Coord const& b)
{
    return Coord(a*b.x,a*b.y);
}

Window::Window(int width,int height,string title):m_width(width),m_height(height)
{
    assert(SDL_Init(SDL_INIT_VIDEO)>=0);

    m_win=SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,m_width,m_height,SDL_WINDOW_SHOWN);
    assert(m_win!=0);

    m_surf=SDL_GetWindowSurface(m_win);
}

Coord Window::startLoop()
{
    while(true)
    {
        if(SDL_WaitEvent(&m_event))
        {
            if(m_event.window.event==SDL_WINDOWEVENT_CLOSE||(m_event.type==SDL_KEYDOWN&&m_event.key.keysym.scancode==SDL_SCANCODE_ESCAPE))
                return Coord(-1,-1);
            if(m_event.type==SDL_MOUSEBUTTONDOWN)
                return Coord(m_event.motion.x,m_event.motion.y);
        }
    }
}

Uint32* Window::getPixel(int x,int y)
{
    return (Uint32*)(m_surf->pixels+x*m_surf->format->BytesPerPixel+y*m_surf->pitch);
}

bool Window::setPixel(int x,int y,Color color)
{
    if(x<m_width&&y<m_height&&x>=0&&y>=0)
    {
        *getPixel(x,y)=SDL_MapRGB(m_surf->format,color.r,color.g,color.b);
        return true;
    }
    return false;

}

void Window::update()
{
    SDL_UpdateWindowSurface(m_win);
}

void Window::save(string file)
{
    SDL_SaveBMP(m_surf,file.c_str());
}

void Window::clean(Color color)
{
    SDL_FillRect(m_surf,NULL,SDL_MapRGB(m_surf->format,color.r,color.g,color.b));
}

void Window::segment(Coord a,Coord b,Color color)
{
    Coord vect;
    Coord dir(b.x-a.x,b.y-a.y);
    Coord pos=a;
    double norm(sqrt(dir.x*dir.x+dir.y*dir.y));
    dir.x/=norm;
    dir.y/=norm;
    while(norm>0.9)
    {
        vect.x=b.x-pos.x;
        vect.y=b.y-pos.y;

        norm=vect.x*vect.x+vect.y*vect.y;

        pos.x+=dir.x;
        pos.y+=dir.y;

        if(pos.x>=0&&pos.x<m_width&&pos.y>=0&&pos.y<m_height)
            setPixel(pos.x,pos.y,color);
    }
}

Window::~Window()
{
    SDL_DestroyWindow(m_win);
    SDL_Quit();
}
