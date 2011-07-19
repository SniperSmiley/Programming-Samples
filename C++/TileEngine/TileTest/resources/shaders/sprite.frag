
uniform sampler2D Texture;

//simple shader
void main()
{
  gl_FragColor = texture2D( Texture, gl_TexCoord[0].st );
}
