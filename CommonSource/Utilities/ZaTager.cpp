#include "ZaTager.h"

CZaTager::CZaTager( void )
	: bufferLen(256)
	, buffer(NULL)
{
	buffer = new char [ bufferLen ];
	memset(buffer, 0, bufferLen);
}

CZaTager::~CZaTager( void ) {
	if ( buffer ) delete [] buffer;
}

void CZaTager::ExtendBuffer( const unsigned int& length ) {

	if ( buffer ) delete [] buffer;

	bufferLen = length + 10;
	buffer = new char [ bufferLen ];
	memset(buffer, 0, bufferLen);
}

const char* CZaTager::GetZaTag( vector<Alignment>& ar1, vector<Alignment>& ar2 ) {
	
	char* zaPtr = buffer;
	unsigned int len = 0;
	len = sprintf( zaPtr, "1");
	zaPtr += len;
	
	for ( vector<Alignment>::iterator ite = ar1.begin(); ite != ar1.end(); ++ite ) {
		len = sprintf( zaPtr, "<");
		zaPtr += len;
		len = sprintf( zaPtr, "%s;", ite->ReferenceName );
		zaPtr += len;
		len = sprintf( zaPtr, "%u;", ite->ReferenceBegin + 1 );
		zaPtr += len;
		len = sprintf( zaPtr, "%u;", ite->Quality);
		zaPtr += len;
		char strand = ( ite->IsReverseStrand ) ? '-' : '+';
		len = sprintf( zaPtr, "%c;", strand );
		zaPtr += len;
		const char* pCigar = cigarTager.GetCigarTag( ite->Reference.CData(), ite->Query.CData(), ite->Reference.Length() );
		len = strlen( pCigar );
		memcpy( zaPtr, pCigar, len );
		zaPtr += len;
		len = sprintf( zaPtr, ">");
		zaPtr += len;
	}


	len = sprintf( zaPtr, "2");
	zaPtr += len;

	for ( vector<Alignment>::iterator ite = ar2.begin(); ite != ar2.end(); ++ite ) {
		len = sprintf( zaPtr, "<");
		zaPtr += len;
		len = sprintf( zaPtr, "%s;", ite->ReferenceName );
		zaPtr += len;
		len = sprintf( zaPtr, "%u;", ite->ReferenceBegin + 1 );
		zaPtr += len;
		len = sprintf( zaPtr, "%u;", ite->Quality);
		zaPtr += len;
		char strand = ( ite->IsReverseStrand ) ? '-' : '+';
		len = sprintf( zaPtr, "%c;", strand );
		zaPtr += len;
		const char* pCigar = cigarTager.GetCigarTag( ite->Reference.CData(), ite->Query.CData(), ite->Reference.Length() );
		len = strlen( pCigar );
		memcpy( zaPtr, pCigar, len );
		zaPtr += len;
		len = sprintf( zaPtr, ">");
		zaPtr += len;
	}
	
	*zaPtr = 0;

	return buffer;
}


const char* CZaTager::GetZaTag( const Alignment& query, const Alignment& mate, const bool& isFirstMate ) {
	
	char* zaPtr = buffer;
	unsigned int len = 0;

	//CCigarTager cigarTager;

	Alignment al1, al2;
	if ( isFirstMate ) {
		al1 = query;
		al2 = mate;
	} else {
		al1 = mate;
		al2 = query;
	}

	// read 1
	len = sprintf( zaPtr, "<");
	zaPtr += len;
	if ( isFirstMate )
		len = sprintf( zaPtr, "@;");
	else
		len = sprintf( zaPtr, "=;");
	zaPtr += len;
	len = sprintf( zaPtr, "%u;%u;", al1.Quality, al1.NextBestQuality );
	zaPtr += len;
	if ( al1.SpecialCode.size() != 0 )
		len = sprintf( zaPtr, "%s;%u;", al1.SpecialCode.c_str(), al1.NumMapped );
	else
		len = sprintf( zaPtr, ";%u;", al1.NumMapped );
	zaPtr += len;
	if ( isFirstMate )
		len = sprintf( zaPtr, ">" );
	else 
		len = sprintf( zaPtr, "%s>", cigarTager.GetCigarTag( al1.Reference.CData(), al1.Query.CData(), al1.Reference.Length() ) );
	zaPtr += len;

	// read 2
	len = sprintf( zaPtr, "<");
	zaPtr += len;
	if ( !isFirstMate )
		len = sprintf( zaPtr, "@;");
	else
		len = sprintf( zaPtr, "=;");
	zaPtr += len;
	len = sprintf( zaPtr, "%u;%u;", al2.Quality, al2.NextBestQuality );
	zaPtr += len;
	if ( al2.SpecialCode.size() != 0 )
		len = sprintf( zaPtr, "%s;%u;", al2.SpecialCode.c_str(), al2.NumMapped );
	else
		len = sprintf( zaPtr, ";%u;", al2.NumMapped );
	zaPtr += len;
	if ( !isFirstMate )
		len = sprintf( zaPtr, ">" );
	else 
		len = sprintf( zaPtr, "%s>", cigarTager.GetCigarTag( al2.Reference.CData(), al2.Query.CData(), al2.Reference.Length() ) );
	zaPtr += len;
	
	*zaPtr = 0;

	return buffer;
}
