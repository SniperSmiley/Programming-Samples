#include <iostream>
#include <string>
#include <iomanip>

#include "mpirxx.h"
#include <vector>

typedef mpz_class mZ;
mZ Totient( mZ Number );

mZ mod_exp( mZ Input, const mZ &Exponent, const mZ &Modulus )
{
  if ( Exponent < 0 )
    return mod_exp( Input, Totient( Modulus ) + Exponent, Modulus );
  mZ Output = 1;

  int Size = Exponent.__get_mp()->_mp_size;
  mp_limb_t *ExIter = Exponent.__get_mp()->_mp_d,
    *ExEnd  = Exponent.__get_mp()->_mp_d + Size;
  std::vector<bool> ExpVec( Size * sizeof( mp_limb_t ) * 8 );
  for ( unsigned i = 0; ExIter != ExEnd; ++ExIter )
  {
    for ( unsigned j = i, n = *ExIter; n;  n >>= 1, ++j )
      ExpVec[j] = ( ( n & 1 ) == 1 );
    i += sizeof( mp_limb_t ) * 8;
  }

  mZ Powered = Input % Modulus;
  std::vector<bool>::iterator VEIter = ExpVec.begin(), VEEnd = ExpVec.end();
  for ( ; VEIter != VEEnd; ++VEIter )
  {
    if ( *VEIter )
      Output = Powered * Output % Modulus;
    Powered = ( Powered * Powered ) % Modulus;
  }

  return Output;
}

const mZ Zero = 0;
const mZ One = 1;
const mZ Three = 3;
mZ Totient( mZ Number )
{
  if ( Number == Zero )
    return Zero;

  mZ Output = One;
  if ( ( Number & One ) == Zero )
  {
    Number >>= 1;
    while ( mZ( Number & One ) == Zero )
      Number >>= 1, Output <<= 1;
  }
  if ( mZ( Number % Three ) == Zero )
  {
    Number /= Three;
    Output <<= 1;
    while ( mZ( Number % Three ) == Zero )
      Number /= Three, Output *= Three;
  }

  mZ PrimeSqrd = 25;
  mZ PrimeSqrdStep = 24;//( 11^2 - 5^2 ) - 5^2
  mZ PrimeSqrdStepStep = 72;
  mZ PrimeStep = 6;
  for ( mZ Prime = 5, Prime2 = 7; PrimeSqrd <= Number;
    Prime += PrimeStep, Prime2 += PrimeStep,
    PrimeSqrd += ( PrimeSqrdStep += PrimeSqrdStepStep ) )
  {
    // this will never be true unless p is prime
    if ( mZ( Number % Prime ) == Zero )
    {
      mZ Divisor = Prime * Prime;
      while ( mZ( Number % Divisor ) == Zero )
        Divisor *= Prime;
      Divisor /= Prime;
      Number /= Divisor;
      Output *= ( Divisor / Prime ) * ( Prime - 1 );
      //Number /= Prime;
      //Output *= Prime - 1;//the totient of Prime
      //while ( mZ( Number % Prime ) == Zero )
      //  Number /= Prime, Output *= Prime;
    }
    if ( mZ( Number % Prime2 ) == Zero )
    {
      mZ Divisor = Prime2 * Prime2;
      while ( mZ( Number % Divisor ) == Zero )
        Divisor *= Prime2;
      Divisor /= Prime2;
      Number /= Divisor;
      Output *= ( Divisor / Prime2 ) * ( Prime2 - 1 );
    }
  }
  // Number is 1 or a prime
  if ( Number == 1 )
    return Output;

  return Output * ( Number - 1 );
}

// Break me
////////////////////////////////////////////////////////////////////////////////
typedef unsigned char Byte;
const double Log_2tothe32 = 0.04508422;
mZ FromString( std::string &Input )
{
  if ( Input.c_str() == "" )
    return Zero;

  std::vector<Byte> InputVec( Input.begin(), Input.end() );

  std::vector<Byte>::iterator InIter = InputVec.begin(), InEnd = InputVec.end();

  mZ Output = 0;
  for ( ; InIter != InEnd; ++InIter )
  {
    Output <<= 8;
    Output += *InIter;
  }

  return Output;
}

std::string ToString( mZ Input )
{
  if ( Input == Zero )
    return "";

  mZ Multiplier = 1;
  std::string Output;
  for ( ; Input != Zero; Input >>= 8 )
  {
    std::string Digit;
    Digit += static_cast<char>( Input.__get_mp()->_mp_d[0] );
    Digit += Output;
    Output = Digit;
  }

  return Output;
}

mZ Encrypt( const mZ &e, const mZ &n, std::string &m, unsigned ChunkSize = 0 )
{

  mZ Value = FromString( m );
  std::cout << Value << std::endl;
  mZ EncryptedValue = mod_exp( Value, e, n );
  std::cout << EncryptedValue << std::endl;
  return EncryptedValue;
}

#include <fstream>
std::vector<std::string> ReadChunks( const std::string &Filename, unsigned ChunkSize )
{
  std::vector<std::string> Chunks;

  std::fstream File;
  File.open( Filename.c_str(), std::fstream::in | std::fstream::binary );

  if ( File.is_open() )
  {
    File.seekg( 0, std::fstream::end );
    int count = File.tellg();
    File.seekg( 0, std::fstream::beg );

    std::string FileText;
    FileText.resize( count );

    File.read( &*FileText.begin(), count );

    std::string::iterator FiIter = FileText.begin(), FiEnd = FileText.end(), FiIter2, FiLEnd;
    FiLEnd = FiIter + ( count - ( count % ChunkSize ) );
    for ( ; FiIter != FiLEnd; FiIter += ChunkSize  )
      Chunks.push_back( std::string( FiIter, FiIter + ChunkSize ) );

    if ( FiIter != FiEnd )
      Chunks.push_back( std::string( FiIter, FiEnd ) );
    File.close();
  }

  return Chunks;
}

void WriteChunks( const std::string &Filename, const std::vector<std::string> &Chunks )
{
  std::fstream File;
  File.open( Filename.c_str(), std::fstream::out | std::fstream::binary );

  if ( File.is_open() )
  {
    std::vector<std::string>::const_iterator ChIter = Chunks.begin(), ChEnd = Chunks.end();

    for ( ; ChIter != ChEnd; ++ChIter )
    {
      File.write( ChIter->c_str(), ChIter->size() );
    }
    File.close();
  }
}

#include <cmath>
int main( int argc, char *argv[] )
{
  mZ e, d, n, p, q;
  p = "440334654777631";
  q = "440334654777631 4843181118617";
  n = "2132620485891757429969256327";//p * q;
  e = "155523435193";
  d = "1991105462108299987974479257";

  bool Decode = false;

  if ( argc < 4 || argc > 6 )
  {
    std::cout <<
      "Params"                      "\n"
      "  1 : Filename"              "\n"
      "  2 : Modulus"               "\n"
      "  3 : Exponent"              "\n"
      ""                            "\n"
      "Optional Params"             "\n"
      ""                            "\n"
      "Decode"                      "\n"
      "  3 : 0 for no check"        "\n"
      "  4 : Decode Exponent"       "\n"
      ""                            "\n"
      "Modulus Decomposition Decode""\n"
      "  4 : p"                     "\n"
      "  5 : q"                     "\n"
      ""                            "\n";
    return -1;
  }

  std::string Filename = argv[1];
  n = argv[2];
  e = argv[3];
  if ( argc == 5 )
  {
    Decode = true;
    d = argv[4];
    if ( e != 0 )
    {
      if ( mod_exp( mod_exp( n - 1, e, n ), d, n ) != ( n - 1 ) )
      {
        std::cout <<
          "The Decode Exponent check failed."                      "\n"
          "The Decode Exponent is incorrect, please check your input.\n"
          "Set Exponent to 0 for no check."                        "\n";
      }
      else
      {
        std::cout <<
          "The Decode Exponent check passed."                "\n"
          "Set Exponent to 0 for no check for faster decoding.\n";
      }
    }
  }
  else if ( argc == 6 )
  {
    Decode = true;
    p = argv[4];
    q = argv[5];
    if ( p * q != n )
    {
      std::cout << " p * q must be the decomposition of the Modulus." << std::endl;
      return -2;
    }
    d = mod_exp( e, -1, ( p - 1 ) * ( q - 1 ) );
    std::cout << "Use "<< d << " as the Decode Exponent for faster decoding." << std::endl;
  }  

  unsigned ChunkSize = 0;
  for ( mZ BasePower = n; mZ( BasePower >>= 8 ) != Zero; )
    ++ChunkSize;
  unsigned ChunkMax = ChunkSize + 1;
  std::vector<std::string> Chunks;

  std::vector<std::string>::iterator ChIter, ChEnd;
  if ( Decode )
  {
    Chunks = ReadChunks( Filename, ChunkMax );
    ChIter = Chunks.begin(), ChEnd = Chunks.end();
    for ( ; ChIter != ChEnd; ++ChIter )
    {
      ( *ChIter ) = ToString( mod_exp( FromString( *ChIter ), d, n ) ); 
    }

    //remove padding
    std::string& temp = Chunks.back();
    int pos = temp.size();
    while ( --pos )
    {
      if ( temp[pos] == temp[pos-1] )
        continue;
      if ( temp[pos] == temp[pos-1] + 1 )
        temp = temp.substr(0, pos);
      break;
    }

    if ( pos == 0 )
    {
      int size = Chunks.size();
      if ( size > 2 && temp[0] == *Chunks[size - 2].rbegin() + 1 )
        Chunks.pop_back();
    }
    //end remove padding

    WriteChunks( std::string( Filename.begin(), Filename.end() - 4 ) += ".dcrp", Chunks );
    return 0;
  }

  Chunks = ReadChunks( Filename, ChunkSize );
  ChIter = Chunks.begin(), ChEnd = Chunks.end();

  //padding 
  bool capPad = true;
  for ( ; ChIter != ChEnd; ++ChIter )
  {
    std::string Temp = *ChIter;
    if ( Temp.size() < ChunkSize )
    {
      capPad = false;
      Temp += std::string( ChunkSize - Temp.size(), Temp[Temp.size() - 1] + 1 );
    }
    *ChIter = Temp;
  }
  if ( capPad )
  {
    std::string& temp = Chunks.back();

    int pos = temp.size();
    while ( --pos )
    {
      if ( temp[pos] == temp[pos-1] )
        continue;
      break;
    }
    if ( ( pos && temp[pos - 1] == temp[pos] - 1 )
      || temp[0] == *Chunks[Chunks.size() - 2].rbegin() + 1 )
      Chunks.push_back( std::string( ChunkSize, temp[pos] + 1 ) );
  }
  //end padding

  ChIter = Chunks.begin(), ChEnd = Chunks.end();
  for ( ; ChIter != ChEnd; ++ChIter )
  {
    std::string Temp = ToString( mod_exp( FromString( *ChIter ), e, n ) );
    if ( Temp.size() < ChunkMax )
      Temp = ( std::string( ChunkMax - Temp.size(), 0 ) += Temp );
    *ChIter = Temp;
  }

  WriteChunks( Filename += ".crp", Chunks );
  std::cout << "Press any key to continue . . . " << std::endl;
}

////Key 1
//int main( void )
//{
//  mZ e, d, n, p, q;
//  p = "21649";
//  q = "513239";
//  n = "11111111111";//p * q;
//  e = "65993";
//  d = "1232394617";//mod_exp( e, -1, ( p - 1 ) * ( q - 1 ) );
//}

////Key 2
//int main( void )
//{
//  mZ e, d, n, p, q;
//  p = "179951";
//  q = "2298041";
//  n = "413534775991";//p * q;
//  e = "65993";
//  d = "393611712857";//mod_exp( e, -1, ( p - 1 ) * ( q - 1 ) );
//}

////Key 3
//int main( void )
//{
//  mZ e, d, n, p, q;
//  p = "535006138814359";
//  q = "61654440233248340616559";
//  n = "32985504009950862202081654855302370681";//p * q;
//  e = "989246133920466898406881225271058613";
//  d = "1868569";//mod_exp( e, -1, ( p - 1 ) * ( q - 1 ) );
//}

////Totient Test
//int main( void )
//{
//  mZ e, d, n, p, q;
//  p = "440334654777631";
//  q = "4843181118617";
//  //n = "2132620485891757429969256327";//p * q;
//  e = "155523435193";
//  d = "1991105462108299987974479257";
//  if ( d == mZ( mod_exp( e, -1, ( p - 1 ) * ( q - 1 ) ) ) )
//    std::cout << "pass" << std::endl;
//  else
//    std::cout << "fail" << std::endl;
//}
