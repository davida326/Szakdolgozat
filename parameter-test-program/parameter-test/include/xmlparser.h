#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <libxml/parser.h> /* debian: # apt install libxml2-dev */
#include <string.h>        
#include <stdio.h>
typedef struct configList{
    xmlNode *element;
    char value[1024];
    struct configList *next;
}configList;



void writeNode(xmlNode *node,char *nodeName,char *value);
void freeList(configList *head);
void writeConfig(char *location);
void searchForDefaultEntry(xmlNode *node,char *nodeName,int *defaultEntry);
void searchForNodeValue(xmlNode *node,xmlDoc *doc,char *nodeName,char *actualValue);
void insertNode (xmlNodePtr cur, char *keyword);
void searchForNodes(xmlNode *node,xmlDoc *doc,char *nodeName,configList **head);
void searchForNodeValues(xmlNode *node,xmlDoc *doc,char *nodeName,configList **head,int *num);
void getValueFromFile(char *location,char *nodeName,char *val);

#endif