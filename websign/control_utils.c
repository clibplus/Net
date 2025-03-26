#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../web.h"

void DestructControls(Control *c) {    
    for(int i = 0; i < c->SubControlCount; i++) {
        if(!c->SubControls[i])
            break;

        DestructControls((Control *)c->SubControls[i]);   
    }

    c->Destruct(c, 0, 1);
}

Control *stack_to_heap(Control c) {
    Control *parent = CreateControl(c.Tag, c.Class, c.ID, c.Text, NULL);
    parent->Parent = c.Parent;
    parent->Name = (!c.Name ? NULL : strdup(c.Name));
    parent->Type = (!c.Type ? NULL : strdup(c.Type));
    parent->href = (!c.href ? NULL : strdup(c.href));
    parent->Data = (!c.Data ? NULL : strdup(c.Data));
    parent->Script = (!c.Script ? NULL : strdup(c.Script));

    parent->sAppend            = Control__AppendStackControl;
    parent->Append             = AppendControl;
    parent->Insert             = Control__AppendControlAt;
    parent->AppendIn           = Control__AppendControlIn;
    parent->AppendCSS          = AppendCSS;
    parent->Construct          = ConstructControl;
    parent->Destruct           = DestructControl;

    if(c.CSS)
        for(int i = 0; c.CSS[i] != NULL; i++)
            AppendCSS(parent, c.CSS[i]);
    
    if(c.SubControls) {
        for(int i = 0; c.SubControls[i] != NULL; i++) {
            if(!c.SubControls[i])
                break;

            parent->sAppend(parent, *(Control *)c.SubControls[i]);
            if(c.SubControls != NULL)
                stack_to_heap(*(Control *)c.SubControls[i]);
        }
    }

    return parent;
}

String DumpControls(Control *controls, int nestingLevel) {
    String ControlList = NewString(NULL);
    
    /* Add Nested Tabs */
    for (int tabs = 0; tabs < nestingLevel; tabs++) {
        ControlList.AppendString(&ControlList, "\t");
    }
    
    printf("%s\n", FindTag(controls));
    printf("%s\n", !controls->Text ? "NULL" : controls->Text);
    printf("%s\n", !controls->ID ? "NULL" : controls->ID);
    printf("%s\n", !controls->Class ? "NULL" : controls->Class);
    String info = control2str(controls);
    ControlList.AppendArray(&ControlList, (const char *[]){info.data, "\n", NULL});
    info.Destruct(&info);
    
    if(!controls->SubControls) {
        ControlList.data[ControlList.idx] = '\0';
        return ControlList;
    }
    
    int idx = 0;
    while(controls->SubControls[idx] != NULL) {
        if(!controls->SubControls[idx])
            break;

        Control *CurrentControl = controls->SubControls[idx];
        
        /* Add Nested Tabs */
        for (int tabs = 0; tabs < nestingLevel; tabs++) {
            ControlList.AppendString(&ControlList, "\t");
        }

        String nest = DumpControls(CurrentControl, nestingLevel + 1);
        if(nest.idx < 1)
            break;

        ControlList.AppendArray(&ControlList, (const char *[]){nest.data, "\n", NULL});
        nest.Destruct(&nest);
        idx++;
    }

    ControlList.data[ControlList.idx] = '\0';
    return ControlList;
}