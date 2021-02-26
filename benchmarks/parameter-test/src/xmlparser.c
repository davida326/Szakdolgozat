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

void writeConfig(char *location){               /* futtatás root jogosultsággal                    */
                                                /* phoronix config fájl, /etc jegyzékben található */
                                                /* /etc/phoronix-test-suite.xml                    */
    xmlDoc *doc;
    xmlNodePtr *root;

    doc = xmlParseFile(location);           //fájl megnyitása
    root = xmlDocGetRootElement(doc);       //gyökérelem eltárolása

    writeNode(root,"DynamicRunCount","FALSE");      //nagy szórás esetén plussz méréseket végez(ez néha 40+ mérést is jelent)
    writeNode(root,"SaveResults","TRUE");           //Előre beállítok pár paramétert, hogy a folyamatos futást ne állítsák meg
    writeNode(root,"OpenBrowser","FALSE");          
    writeNode(root,"UploadResults","FALSE");
    writeNode(root,"PromptForTestIdentifier","FALSE");
    writeNode(root,"PromptForTestDescription","FALSE");
    writeNode(root,"PromptSaveName","FALSE");
    writeNode(root,"RunAllTestCombinations","FALSE");
    writeNode(root,"Configured","TRUE");

    xmlSaveFormatFile (location, doc, 0); //átírt fájl mentése
	xmlFreeDoc(doc);
}
void searchForDefaultEntry(xmlNode *node,char *nodeName,int *defaultEntry){
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){
        *defaultEntry=1;        
    }
    
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
        sprintf(actualValue,"%s",key);                            /* az értékét eltárolja a megadott pointer-be */
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
void searchForNodes(xmlNode *node,xmlDoc *doc,char *nodeName,configList **head){
    if(!xmlStrcmp(node->name,(const xmlChar *)nodeName)){
          /* kikeresi a megfelelő nevű node-ot          */
        if((*head)==NULL){
            (*head)=malloc(sizeof(configList));
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
        key = xmlNodeListGetString(doc,node->xmlChildrenNode,1);    /* kikeresi a megfelelő nevű node-ot          */
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
    xmlDoc *doc;                                    /* a paraméterként megkapott fájlútvonalat megnyitja,       */
    xmlNode *root;                                  /* az előző függvény segítségével, eltárolja az eredményt   */
    doc = xmlParseFile(location);                   /* az eredményt továbbadja a kapott char *val-nak           */
    root = xmlDocGetRootElement(doc);
    searchForNodeValue(root,doc,nodeName,val);    
}