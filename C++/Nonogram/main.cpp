/* Start Header -------------------------------------------------------
File Name: main.cpp
Purpose: A helper program for solving nonograms
Language: C++, Visual Studio 2008
Platform: Visual Studio 9, Windows
Author: Nathan Anderson
Email: SniperSmiley@gmail.com
- End Header --------------------------------------------------------*/
#include <glfw.h>
#include <cstdlib> 

void DrawBox( unsigned x, unsigned y, unsigned w, unsigned h );
void DrawPixel( unsigned x, unsigned y );

static unsigned PixelScale = 16;
int main( int argc, char* argv[] )
{
  int w = 32, h = 32;

  if ( argc >= 2 )
    w = atoi( argv[1] );
  if ( argc >= 3 )
    h = atoi( argv[2] );
  if ( argc == 4 )
    PixelScale = atoi( argv[3] );

  bool** pixel = new bool*[h];
  for ( int y = 0; y != h; ++y )
  {
    pixel[y] = new bool[w];
    for ( int x = 0; x != w; ++x )
      pixel[y][x] = false;
  }

  int width = w * PixelScale + 1, height = h * PixelScale + 1;

  glfwInit();
  glfwOpenWindow( width, height, 8, 8, 8, 8, 8, 8, GLFW_WINDOW );
  glfwSwapInterval( 1 );

  glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0, width, 0, height, -100, 100 );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glTranslatef( 0.375, 0.375, 0 );

  bool Down = false;
  while ( !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED ) )
  {
    if ( !Down && glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) )
    {
      Down = true;
      int x,y;
      glfwGetMousePos( &x, &y );
      x /= PixelScale, y /= PixelScale;
      y = h - y - 1;
      pixel[y][x] = !pixel[y][x];
    }
    else
      Down = ( glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) != 0 );

    glClear( GL_COLOR_BUFFER_BIT );

    //Draw Lines
    glColor4ub( 0x00, 0x00, 0x00, 0xff );
    glBegin( GL_LINES );
    for ( int i = 0; i < width; i += PixelScale )
    {
      glVertex2i( i, 0 );
      glVertex2i( i, height );
    }

    for ( int i = 0; i < height; i += PixelScale )
    {
      glVertex2i( 0, i );
      glVertex2i( width, i );
    }
    glEnd();

    //Draw Pixels
    glColor4ub( 0x00, 0xff, 0x00, 0xff );
    for ( int y = 0; y != h; ++y )
      for ( int x = 0; x != w; ++x )
        if ( pixel[y][x] )
          DrawPixel( x, y );

    glfwSwapBuffers();
  }

  glfwTerminate();
}

void DrawBox( unsigned x, unsigned y, unsigned w, unsigned h )
{
  glBegin( GL_QUADS );
  unsigned pixelX = x * PixelScale;
  unsigned pixelY = y * PixelScale;
  unsigned pixelW = w * PixelScale;
  unsigned pixelH = h * PixelScale;
  glVertex2i( pixelX + 1, pixelY + 1 );
  glVertex2i( pixelX + pixelW, pixelY + 1 );
  glVertex2i( pixelX + pixelW, pixelY + pixelH );
  glVertex2i( pixelX + 1, pixelY + pixelH );

  glEnd();
}
void DrawPixel( unsigned x, unsigned y )
{
  DrawBox( x, y, 1, 1 );
}