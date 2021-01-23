#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <libxml/parser.h> /* debian: # apt install libxml2-dev */
#include <string.h>        
#include <stdio.h>

void writeNode(xmlNode *node,xmlDoc *doc,char *nodeName,char *value);
void writeConfig(char *location);
void searchForNodeValue(xmlNode *node,xmlDoc *doc,char *nodeName,char *actualValue);
void getValueFromFile(char *location,char *val);

#endif