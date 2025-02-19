#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../web.h"

CSS *CreateCSS(const char *class, int selector, const char **data) {
    CSS *style = (CSS *)malloc(sizeof(CSS));
    *style = (CSS){
        .Class = strdup(class),
        .Data = (char **)malloc(sizeof(char *) * 1),
        .DataCount = 0,
        .Selector = selector
    };

    while(data[style->DataCount] != NULL) {
        AppendCSS(style, data[style->DataCount]);
    }

    return style;
}

int AppendCSS(CSS *style, const char *q) {
    if(!style || !q)
        return 0;

    style->Data[style->DataCount] = strdup(q);
    style->DataCount++;
    style->Data = (char **)realloc(style->Data, sizeof(char *) * (style->DataCount + 1));

    return 1;
}

void DestroyCSS(CSS *style) {
    if(style->Class)
        free(style->Class);

    if(style->Data) {
        for(int i = 0; i < style->DataCount; i++)
            if(style->Data[i])
                free(style->Data[i]);

        free(style->Data);
    }
    
    free(style);
}