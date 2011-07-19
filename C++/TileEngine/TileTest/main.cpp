/* Start Header ----------------------------------------------------------------
File Name: main.cpp
Purpose: Create a shader to draw a tiled background.
Language: C++, Visual Studio 2008
Platform: Visual Studio 9, Windows
Author: Nathan James Anderson
Email: SniperSmiley@gmail.com
- End Header -----------------------------------------------------------------*/
#include "GL/GLee.h"
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "SOIL/SOIL.h"
#include "TimeClass.h"
#include "JSprite.h"
#include "JImage.h"
#include "Input.h"

#pragma comment( lib, "SDL/lib/SDL.lib" )
#pragma comment( lib, "SDL/lib/SDLmain.lib" )

#pragma comment( lib, "SOIL.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )


//This data and the init should go into a class.
const int SCREEN_BPP = 32;
float screenWidth = 1280;
float screenHeight = 720;
Input input;

void initialize( void )
{
  srand( Time::Clock() );

  std::string windowName = "Game Title";
  bool fullscreen = false;
  unsigned windowFlags = SDL_OPENGL;

  if ( fullscreen )
    windowFlags |= SDL_FULLSCREEN;

  SDL_Init( SDL_INIT_EVERYTHING );
  SDL_SetVideoMode( screenWidth, screenHeight, SCREEN_BPP, windowFlags );
  SDL_WM_SetCaption( windowName.c_str(), windowName.c_str() );

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  input.Init();
  glMatrixMode( GL_PROJECTION );
  glOrtho( 0, screenWidth, 0, screenHeight, 1, -1 );
}

int main( int argc, char *argv[] )
{
  initialize();
  //Using my brothers stuff just to make it easier.
  Shader tile( "resources/shaders/tile.vert", "resources/shaders/tile.frag" );

  JImageManager imageManager;
  int tileMap = imageManager.Create( "resources/tile.bmp" );
  int levelMap = imageManager.Create( "resources/levelTest.bmp" );
  int charHandle = imageManager.Create( "resources/Character.png" );

  tile.uniform( "tileMap", 0 );
  tile.uniform( "textureMap", 1 );
  tile.uniform( "tilesWide", 16.0f );
  tile.uniform( "tilesHigh", 16.0f );

  JSprite characterSprite;
  characterSprite.Init();

  int posX = 0;
  int posY = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
  float pixelTest = 0.0f;
  int size = 1024;
  tile.uniform( "size", size );

  //Setting the filtering to nearest is how this is able to work.
  glBindTexture( GL_TEXTURE_2D, tileMap );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glBindTexture( GL_TEXTURE_2D, levelMap );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );


  bool active = true;
  SDL_Event event;
  Time::Timer timer;
  timer.Start();
  while ( active )
  {
    //exit 
    while ( SDL_PollEvent( &event ) )
      if ( event.type == SDL_QUIT )
        active = false;
    input.Update();
    if ( input.KeyPressed( SDLK_ESCAPE ) )
      active = false;

    //simple movement
    posX += input.KeyPressed( SDLK_RIGHT ) - input.KeyPressed( SDLK_LEFT );
    posY += input.KeyPressed( SDLK_UP ) - input.KeyPressed ( SDLK_DOWN );


    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //draw test
    tile.BindProgram();
    glDisable( GL_BLEND );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tileMap );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, levelMap );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glTranslatef( -posX, -posY, 0.0f );

    glBegin( GL_QUADS );
    glTexCoord2i( 0, 1 );
    glVertex3i( 0, 0, -1 );
    glTexCoord2i( 1, 1 );
    glVertex3i( size, 0, -1 );
    glTexCoord2i( 1, 0 );
    glVertex3i( size, size, -1 );
    glTexCoord2i( 0, 0 );
    glVertex3i( 0, size, -1 );
    glEnd();

    glPopMatrix();

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    Matrix tran = characterSprite.BuildTransformation( Vec2( posX, posY ),
      Vec2( 0.0f, 0.0f ), Vec2( 64, 64 ) );
    characterSprite.Draw( charHandle, tran,
      JRect( 0.25, 0.0, 0.66666666, 0.33333333 ) );
    characterSprite.DrawBatch();


    SDL_GL_SwapBuffers();

    //simple frame limiter
    timer.Stop();
    float frameTime = timer.GetFrame();
    if ( frameTime < 1.0 / 60.0 )
      Sleep( ( 1.0 / 60.0 - frameTime ) * 1000 );
    timer.Start();
  }

  return 0;
}
