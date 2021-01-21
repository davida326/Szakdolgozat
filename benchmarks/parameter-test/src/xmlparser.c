#include "xmlparser.h"

void writeNode(xmlNode *node,xmlDoc *doc,char *nodeName,char *value){
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){   /* rekurzívan kikeresi a megfelelő nevü node-ot       */
        xmlNodeSetContent(node,value);                      /* amint megtalálta, módosítja azt a megadott értékre */
    }
    xmlNode *children;                          /* node -> gyerek -> gyerek -> NULL !STOP!;*/
    if( node->children != NULL )                /*      NextIteration: node = node->next   */
        children = node->children;
    while( children != NULL){
            if(children->type == 1){
                if( children->children != NULL )
                writeNode(children,doc,nodeName,value);                 
            }
        children = children->next;
    }
}

void writeConfig(char *location){               /* futtatás root jogosultsággal                    */
                                                /* phoronix config fájl, /etc jegyzékben található */
                                                /* /etc/phoronix-test-suite.xml                    */
    xmlDoc *doc;
    xmlNodePtr *root;

    doc = xmlParseFile(location);           //fájl megnyitása
    root = xmlDocGetRootElement(doc);       //gyökérelem eltárolása

    writeNode(root,doc,"DynamicRunCount","FALSE");      //nagy szórás esetén plussz méréseket végez(ez néha 40+ mérést is jelent)
    writeNode(root,doc,"SaveResults","TRUE");           //Előre beállítok pár paramétert, hogy a folyamatos futást ne állítsák meg
    writeNode(root,doc,"OpenBrowser","FALSE");          
    writeNode(root,doc,"UploadResults","FALSE");
    writeNode(root,doc,"PromptForTestIdentifier","FALSE");
    writeNode(root,doc,"PromptForTestDescription","FALSE");
    writeNode(root,doc,"PromptSaveName","FALSE");
    writeNode(root,doc,"RunAllTestCombinations","FALSE");
    writeNode(root,doc,"Configured","TRUE");

    xmlSaveFormatFile (location, doc, 0); //átírt fájl mentése
	xmlFreeDoc(doc);
}

void searchForNodeValue(xmlNode *node,xmlDoc *doc,char *nodeName,char *actualValue){
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){
        xmlChar *key;
        key = xmlNodeListGetString(doc,node->xmlChildrenNode,1);    /* kikeresi a megfelelő nevű node-ot          */
        sprintf(actualValue,"%s",key);                              /* az értékét eltárolja a megadott pointer-be */
        xmlFree(key);                                               
    }
    xmlNode *children;
    if( node->children != NULL )    
        children = node->children;
    while( children != NULL){
            if(children->type == 1){
                if( children->children != NULL )
                searchForNodeValue(children,doc,nodeName,actualValue);                 
            }
        children = children->next;
    }
}

void getValueFromFile(char *location,char *val){
    xmlDoc *doc;                                    /* a paraméterként megkapott fájlútvonalat megnyitja,       */
    xmlNode *root;                                  /* az előző függvény segítségével, eltárolja az eredményt   */
    doc = xmlParseFile(location);                   /* az eredményt továbbadja a kapott char *val-nak           */
    root = xmlDocGetRootElement(doc);
    char actualValue[50];
    searchForNodeValue(root,doc,"Value",actualValue);
    strcpy(val,actualValue);
}