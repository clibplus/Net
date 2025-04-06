#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <str.h>
#include <Net/web.h>

typedef enum AlgoControls {
    ALGO_BUTTON,
    ALGO_INPUT,
    ALGO_TEXTBOX
} AlgoControls;

Control *CreateButton(ControlTag tag, const char *class, const char *id, const char *text, const char *path, Control **controls) {
    if(tag != ALGO_INPUT && tag != ALGO_BUTTON) {
        return NULL;
    }

    Control *n = CreateControl(tag, (!class ? NULL : class), (!id ? NULL : id), NULL, (!controls ? NULL : controls));
    
    if(!text) 
        return n;

    if(tag == INPUT_TAG) {
        n->Data = CreateString((char *[]){"value=\"", (char *)text, "\"", NULL});
    } else {
        n->Text = (char *)text;
    }

    return n;
}

Control *CreateTextbox(const char *class, const char *id, const char *place_holder, Control **controls) {
    Control *n = CreateControl(INPUT_TAG, (!class ? NULL : class), (!id ? NULL : id), NULL, (!controls ? NULL : controls));
    
    if(!place_holder)
        return n;

    n->Data = CreateString((char *[]){"placeholder=\"", (char *)place_holder, "\"", NULL});
    return n;
}

Control *CreateInputForm(const char *class, const char *id, char ***arr) {
    Control *n = CreateControl(DIV_TAG, (!class ? NULL : class), (!id ? NULL : id), NULL, NULL);

    int i = 0;
    if(arr) {
        for(i = 0; arr[i] != NULL; i++) {
            n->Append(n, CreateTextbox((!arr[i][0] ? NULL : arr[i][0]), (!arr[i][1] ? NULL : arr[i][1]), (!arr[i][2] ? NULL : arr[i][2]), NULL));
            
            if(!arr[i + 1]) // If next is NULL leave
                break;
        }
    }

    n->Append(n, CreateButton(INPUT_TAG, (!arr[i][0] ? NULL : arr[i][0]), (!arr[i][1] ? NULL : arr[i][1]), (!arr[i][2] ? NULL : arr[i][2]), NULL));

    return n;
}