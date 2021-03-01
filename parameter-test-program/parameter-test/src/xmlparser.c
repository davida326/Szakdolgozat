#include "xmlparser.h"

void writeNode(xmlNode *node,char *nodeName,char *value){
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){   /* rekurzívan kikeresi a megfelelő nevü node-ot       */
        xmlNodeSetContent(node,value);                      /* amint megtalálta, módosítja azt a megadott értékre */
    }
                                                 /* node -> gyerek -> gyerek -> NULL !STOP!;*/
    if( node->children != NULL ){                /*      NextIteration: node = node->next   */
        xmlNode *children;
        children = node->children;
        while( children != NULL){
                if(children->type == 1){
                    if( children->children != NULL )
                    writeNode(children,nodeName,value);                 
                else return;
                }
            children = children->next;
        }
    }
}
void freeList(configList *head){
    configList *tmp;
    while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

void writeConfig(char *location){               /* A PTS program root-ként való futtátásánál                */
                                                /* a /etc/phoronix-test-suite.xml fájlt fogja felhasználni. */
    xmlDoc *doc;
    xmlNodePtr *root;

    doc = xmlParseFile(location);           //fájl megnyitása
    root = xmlDocGetRootElement(doc);       //gyökérelem eltárolása

    writeNode(root,"DynamicRunCount","FALSE");      //nagy szórás esetén plussz méréseket végez(ez néha 40+ mérést is jelent), ezt szeretném elkerülni emiatt FALSE
    writeNode(root,"SaveResults","TRUE");           //Előre beállítok pár paramétert, hogy a program futását ne állítsák meg
    writeNode(root,"OpenBrowser","FALSE");          //Ezek mind beparaméterezhetők config és környezeti változókon keresztül
    writeNode(root,"UploadResults","FALSE");
    writeNode(root,"PromptForTestIdentifier","FALSE");
    writeNode(root,"PromptForTestDescription","FALSE");
    writeNode(root,"PromptSaveName","FALSE");
    writeNode(root,"RunAllTestCombinations","FALSE");
    writeNode(root,"Configured","TRUE");

    xmlSaveFormatFile (location, doc, 0); //átírt fájl mentése
	xmlFreeDoc(doc);
}
void searchForDefaultEntry(xmlNode *node,char *nodeName,int *defaultEntry){ /* DefaultEntry - néhány teszt több különféle beállítással indítható */
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){                   /* ezekhez beállítható egy DefaultEntry node, amiben mindig az alap  */
        *defaultEntry=1;                                                    /* beállítást fogja indítani. Ezt én amikor megtalálom, felülírom,   */
    }                                                                       /* vagy egyszerűen beszúrom az xml fájlba, a kívánt beállítással     */
    if( node->children != NULL ){
        xmlNode *children;
        children = node->children;
        while( children != NULL){
            if(children->type == 1){
                if( children->children != NULL )
                    searchForDefaultEntry(children,nodeName,defaultEntry);                 
            }
        children = children->next;
        }
    }
    
}

void searchForNodeValue(xmlNode *node,xmlDoc *doc,char *nodeName,char *actualValue){
    
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){
        xmlChar *key;
        key = xmlNodeListGetString(doc,node->xmlChildrenNode,1);    /* kikeresi a megfelelő nevű node-ot          */
        sprintf(actualValue,"%s",key);                              /* az értékét eltárolja a megadott pointer-be */
        xmlFree(key);                                               
    }
    if( node->children != NULL ){
        xmlNode *children;
        children = node->children;
        while( children != NULL){
            if(children->type == 1){
                if( children->children != NULL )
                    searchForNodeValue(children,doc,nodeName,actualValue);  
            }
            children = children->next;
        }
    }
    
}

void insertNode (xmlNodePtr cur, char *keyword) {
	xmlNewTextChild (cur, NULL, "DefaultEntry", keyword);
    return;
}
void searchForNodes(xmlNode *node,xmlDoc *doc,char *nodeName,configList **head){  /* láncolt listába összegyűjti a megadott nevű node-okat */
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){                         /* nagyon fontos rész a beállítások szabályzásában       */
        if((*head)==NULL){
            (*head)=malloc(sizeof(configList));                                   /* rekurzívan keresi */
            (*head)->element=node;
        }
        else{
            configList *link = malloc(sizeof(configList));
            link->element = node;
            link->next = (*head);
            (*head) = link;
        }
    }
    if( node->children != NULL ){
        xmlNode *children;
        children = node->children;
    
    while( children != NULL){
                if(children->type == 1){
                    if( children->children != NULL )
                        searchForNodes(children,doc,nodeName,head);
                }
            children = children->next;
        }
    }
}

void searchForNodeValues(xmlNode *node,xmlDoc *doc,char *nodeName,configList **head,int *num){
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){
        xmlChar *key;
        *num = *num+1;
        key = xmlNodeListGetString(doc,node->xmlChildrenNode,1);    /* kikeresi a megfelelő nevű node értékeket, amiket láncolt listában tárol */
        if((*head)==NULL){
            (*head)=malloc(sizeof(configList));
            sprintf((*head)->value,"%s",key);
        }
        else{
            configList *link = malloc(sizeof(configList));
            sprintf(link->value,"%s",key);
            link->next = (*head);
            (*head) = link;
        }
        xmlFree(key);                                               
    }
    
    if( node->children != NULL ){
        xmlNode *children;
        children = node->children;
        while( children != NULL){
                if(children->type == 1){
                    if( children->children != NULL ){
                        searchForNodeValues(children,doc,nodeName,head,num);
                    }
                }
            children = children->next;
        }
    }
}
void getValueFromFile(char *location,char *nodeName,char *val){
    if( !(access( location, 0 ) == 0)) { // fájl létezésének ellenörzése, ha sajnos nem letezik ilyen teszt
        clear();                            // programnév-x.x.x
        printw("eredmeny fajl nem talalhato\n");
        refresh();
        getch();
        exit(0);
    } 
    xmlDoc *doc;                                    /* a paraméterként megkapott fájlútvonalat megnyitja,       */
    xmlNode *root;                                  /* az előző függvény segítségével, eltárolja az eredményt   */
    doc = xmlParseFile(location);                   /* az eredményt továbbadja a kapott char *val-nak           */
    root = xmlDocGetRootElement(doc);
    searchForNodeValue(root,doc,nodeName,val);    
}
