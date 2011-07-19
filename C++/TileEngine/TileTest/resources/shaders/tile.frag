uniform sampler2D tileMap;
uniform sampler2D textureMap;

uniform float tilesWide;
uniform float tilesHigh;
uniform int size;

void main( void )
{
  vec4 coords = texture2D( textureMap, gl_TexCoord[0].st );
  
  int index = int( coords.r * 255.0 );
  
  float s = mod( index + mod( gl_TexCoord[0].s * tilesWide, 1.0 ), 8.0 ) / 8.0;
  //float s = mod( gl_TexCoord[0].s, 2.0 );
  float t = mod( index / 8 + mod( gl_TexCoord[0].t * tilesHigh, 1.0 ), 8.0 ) / 8.0;
  //float t = mod( gl_TexCoord[0].t, 2.0 );

  gl_FragColor = texture2D( tileMap, vec2( s, t ) );
  return;
}