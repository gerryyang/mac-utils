#include "tinyxml2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace tinyxml2;

int gPass = 0;
int gFail = 0;


template<class T> bool XMLTest(const char* testString, T expected, T found, bool echo = true)
{
	bool pass = ( expected == found );
	if ( pass )
		printf ("[pass]");
	else
		printf ("[fail]");

	if ( !echo )
		printf (" %s\n", testString);
	else
		printf (" %s [%d][%d]\n", testString, static_cast<int>(expected), static_cast<int>(found) );

	if ( pass )
		++gPass;
	else
		++gFail;

	return pass;
}


int example_1()
{
	XMLDocument doc;
	doc.LoadFile("resources/dream.xml");

	return doc.ErrorID();
}

int example_2()
{
	static const char* xml = "<element/>";
	XMLDocument doc;
	doc.Parse( xml );

	return doc.ErrorID();
}

int example_3()
{
	static const char* xml =
		"<?xml version=\"1.0\"?>"
		"<!DOCTYPE PLAY SYSTEM \"play.dtd\">"
		"<PLAY>"
		"<TITLE>A Midsummer Night's Dream</TITLE>"
		"</PLAY>";

	XMLDocument doc;
	doc.Parse( xml );

	XMLElement* titleElement = doc.FirstChildElement( "PLAY" )->FirstChildElement( "TITLE" );
	const char* title = titleElement->GetText();
	printf( "Name of play (1): %s\n", title );

	XMLText* textNode = titleElement->FirstChild()->ToText();
	title = textNode->Value();
	printf( "Name of play (2): %s\n", title );

	return doc.ErrorID();
}

bool example_4()
{
	static const char* xml =
		"<information>"
		"	<attributeApproach v='2' />"
		"	<textApproach>"
		"		<v>2</v>"
		"	</textApproach>"
		"</information>";

	XMLDocument doc;
	doc.Parse( xml );

	int v0 = 0;
	int v1 = 0;

	XMLElement* attributeApproachElement = doc.FirstChildElement()->FirstChildElement( "attributeApproach" );
	attributeApproachElement->QueryIntAttribute( "v", &v0 );

	XMLElement* textApproachElement = doc.FirstChildElement()->FirstChildElement( "textApproach" );
	textApproachElement->FirstChildElement( "v" )->QueryIntText( &v1 );

	printf( "Both values are the same: %d and %d\n", v0, v1 );

	return !doc.Error() && ( v0 == v1 );
}


int main(int argc, char *argv[])
{
	FILE* fp = fopen("resources/dream.xml", "r");
	if (!fp)
	{
		printf("fopen resources/dream.xml err\n");
		exit(1);
	}
	fclose(fp);

	// Load an XML File
	XMLTest("Example-1", 0, example_1());

	// Parse an XML from char buffer
	XMLTest("Example-2", 0, example_2());

	// Get information out of XML
	XMLTest("Example-3", 0, example_3());

	// Read attributes and text information
	XMLTest("Example-4", true, example_4());


	return 0;
}
/*
output:
gerryyang@mba:tinyxml2$./demo 
[pass] Example-1 [0][0]
[pass] Example-2 [0][0]
Name of play (1): A Midsummer Night's Dream
Name of play (2): A Midsummer Night's Dream
[pass] Example-3 [0][0]
Both values are the same: 2 and 2
[pass] Example-4 [1][1]
 */
