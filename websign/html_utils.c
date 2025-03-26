#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../web.h"

#define HTML_TAGS_COUNT 113

#define ENCODED_SYMBOL_COUNT 5

void *HTML_TAGS[][2] = {
    { (void *)NO_TAG,           "NULL" },
    { (void *)HTML_TAG,         "html" },
    { (void *)HEAD_TAG,         "head"},
    { (void *)TITLE_TAG,        "title" },
    { (void *)BODY_TAG,         "body" },
    { (void *)DIV_TAG,          "div" },
    { (void *)H1_TAG,           "h1" },
    { (void *)H2_TAG,           "h2" },
    { (void *)H3_TAG,           "h3" },
    { (void *)P_TAG,            "p" },
    { (void *)A_TAG,            "a" },
    { (void *)PRE_TAG,          "pre" },
    { (void *)BUTTON_TAG,       "button" },
    { (void *)INPUT_TAG,        "input" },
    { (void *)FORM_TAG,         "form" },
    { (void *)IMG_TAG,          "img" },
    { (void *)META_TAG,         "meta" },
    { (void *)LINK_TAG,         "link" },
    { (void *)BASE_TAG,         "base" },
    { (void *)STYLE_TAG,        "style" },
    { (void *)ADDRESS_TAG,      "address" },
    { (void *)ARTICLE_TAG,      "article" },
    { (void *)ASIDE_TAG,        "aside" },
    { (void *)FOOTER_TAG,       "footer" },
    { (void *)HEADER_TAG,       "header" },
    { (void *)HGROUP_TAG,       "hgroup" },
    { (void *)MAIN_TAG,         "main" },
    { (void *)NAV_TAG,          "nav" },
    { (void *)SECTION_TAG,      "section" },
    { (void *)SEARCH_TAG,       "search" },
    { (void *)BLOCKQUOTE_TAG,   "blockquote" },
    { (void *)DD_TAG,           "dd" },
    { (void *)DL_TAG,           "dl" },
    { (void *)DT_TAG,           "dt" },
    { (void *)FIGCAPTION_TAG,   "figcaption" },
    { (void *)FIGURE_TAG,       "figure" },
    { (void *)HR_TAG,           "hr" },
    { (void *)LI_TAG,           "li" },
    { (void *)MENU_TAG,         "menu" },
    { (void *)OL_TAG,           "ol" },
    { (void *)UL_TAG,           "ul" },
    { (void *)ABBR_TAG,         "abbr" },
    { (void *)B_TAG,            "b" },
    { (void *)BDI_TAG,          "bdi" },
    { (void *)BDO_TAG,          "bdo" },
    { (void *)BR_TAG,           "br" },
    { (void *)CITE_TAG,         "cite" },
    { (void *)CODE_TAG,         "code" },
    { (void *)DATA_TAG,         "data" },
    { (void *)DFN_TAG,          "dfn" },
    { (void *)EM_TAG,           "em" },
    { (void *)I_TAG,            "i" },
    { (void *)KBD_TAG,          "kbd" },
    { (void *)MARK_TAG,         "mark" },
    { (void *)Q_TAG,            "q" },
    { (void *)RP_TAG,           "rp" },
    { (void *)RT_TAG,           "rt" },
    { (void *)RUBY_TAG,         "ruby" },
    { (void *)S_TAG,            "s" },
    { (void *)SAMP_TAG,         "samp" },
    { (void *)SMALL_TAG,        "small" },
    { (void *)SPAN_TAG,         "span" },
    { (void *)STRONG_TAG,       "strong" },
    { (void *)SUB_TAG,          "sub" },
    { (void *)SUP_TAG,          "sup" },
    { (void *)TIME_TAG,         "time" },
    { (void *)U_TAG,            "u" },
    { (void *)VAR_TAG,          "var" },
    { (void *)WBR_TAG,          "wbr" },
    { (void *)AREA_TAG,         "area" },
    { (void *)AUDIO_TAG,        "audio" },
    { (void *)MAP_TAG,          "map" },
    { (void *)TRACK_TAG,        "track" },
    { (void *)VIDEO_TAG,        "video" },
    { (void *)EMBED_TAG,        "embed" },
    { (void *)FENCEDFRAME_TAG,  "fencedframe" },
    { (void *)IFRAME_TAG,       "iframe" },
    { (void *)OBJECT_TAG,       "object" },
    { (void *)PICTURE_TAG,      "picture" },
    { (void *)SOURCE_TAG,       "source" },
    { (void *)SVG_TAG,          "svg" },
    { (void *)MATH_TAG,         "math" },
    { (void *)CANVAS_TAG,       "canvas" },
    { (void *)NOSCRIPT_TAG,     "noscript" },
    { (void *)SCRIPT_TAG,       "script" },
    { (void *)DEL_TAG,          "del" },
    { (void *)INS_TAG,          "ins" },
    { (void *)CAPTION_TAG,      "captio" },
    { (void *)COL_TAG,          "col" },
    { (void *)COLGROUP_TAG,     "colgroup" },
    { (void *)TABLE_TAG,        "table" },
    { (void *)TBODY_TAG,        "tbody" },
    { (void *)TD_TAG,           "td" },
    { (void *)TFOOT_TAG,        "tfoot" },
    { (void *)TH_TAG,           "th" },
    { (void *)THEAD_TAG,        "thead" },
    { (void *)TR_TAG,           "tr" },
    { (void *)DATALIST_TAG,     "datalist" },
    { (void *)FIELDSET_TAG,     "fieldset" },
    { (void *)LABEL_TAG,        "label" },
    { (void *)LEGEND_TAG,       "legend" },
    { (void *)METER_TAG,        "meter" },
    { (void *)OPTGROUP_TAG,     "optgroup" },
    { (void *)OPTION_TAG,       "option" },
    { (void *)OUTPUT_TAG,       "output" },
    { (void *)PROGRESS_TAG,     "progress" },
    { (void *)SELECT_TAG,       "select" },
    { (void *)TEXTAREA_TAG,     "textarea" },
    { (void *)DATAILS_TAG,      "datails" },
    { (void *)DIALOG_TAG,       "dialog" },
    { (void *)SUMMARY_TAG,      "summary" },
    { (void *)SLOT_TAG,         "slot" },
    { (void *)TEMPLATE_TAG,     "template" },
    NULL
};

void *EncodedSymbols[][2] = {
    {(void *)' ',      "+"},
    {(void *)'@',      "%40"},
    {(void *)';',      "%3B"},
    {(void *)':',      "%3A"},
    {(void *)'#',      "%23"},
    NULL
};

char *decode_input_symbols(const char *data) {
    if(!data)
        return NULL;

    String n = NewString(data);

    for(int i = 0; i < ENCODED_SYMBOL_COUNT; i++) {
        if(!EncodedSymbols[i])
            break;

        int pos = -1;
        while((pos = n.FindString(&n, EncodedSymbols[i][1])) != -1) {
            int rm_len = strlen(EncodedSymbols[i][1]);
            for(int b = 0; b < rm_len; b++)
                n.TrimAt(&n, pos);
                
            n.InsertAtIdx(&n, pos - 1, ((char)((char *)EncodedSymbols[i][0])));
            pos = -1;
        }
    }

    char *output = strdup(n.data);
    n.Destruct(&n);

    if(strlen(output) > 0)
        return output;

    free(output);
    return NULL;
}

char *FindTag(Control *control) {
    for(int i = 0; i < HTML_TAGS_COUNT; i++) {
        if(!HTML_TAGS[i])
            break;

        if((ControlTag)control->Tag == (ControlTag)HTML_TAGS[i][0])
            return HTML_TAGS[i][1];
    }

    return NULL;
}

ControlTag FindTagType(const char *data) {
    if(!data)
        return 0;
        
    for(int i = 0; i < HTML_TAGS_COUNT; i++)
        if(!strcmp(data, (char *)HTML_TAGS[i][1]))
            return (ControlTag)HTML_TAGS[i][0];

    return 0;
}

char *ConstructTemplate(Control **controls, CSS **styles, int click, int hover, int mouse_track, int keydown, int keyup, int oneline) {
    String template = NewString("<!--\nGenerated Using Websign: https://github.com/clibplus/Net\n@author: @algo1337\n-->\n<html>");
    if(!oneline)
        template.AppendString(&template, "\n");

    int i = 0;
    if(!controls || !controls[0])
        return NULL;

    if(controls[0]->Tag == HEAD_TAG && styles != NULL) {
        String handler = ConstructHandler(click, hover, mouse_track, keydown, keyup);
        if(handler.idx > 0)
            controls[0]->Script = strdup(handler.data);

        String header = ConstructControl(controls[0], 0, oneline);
        template.AppendArray(&template, (const char *[]){header.data, (!oneline ? "\n\n" : NULL), NULL});
        header.Destruct(&header);
        
        char *data = ConstructCSS(styles, oneline);
        template.AppendArray(&template, (const char *[]){data, (!oneline ? "\n\n" : NULL), NULL});
        free(data);

        i = 1;
    }

    for(;controls[i] != NULL; i++) {
        String new = ConstructControl(controls[i], 0, oneline);
        template.AppendArray(&template, (const char *[]){new.data, (!oneline ? "\n\n" : NULL), NULL});
        new.Destruct(&new);
    }
    
    template.AppendString(&template, "</html>\n\n\n");
    template.data[template.idx] = '\0';
    if(template.idx > 0) {
        char *output = strdup(template.data);
        template.Destruct(&template);
        return output;
    }

    template.Destruct(&template);
    return NULL;
}

char *ConstructCSS(CSS **styles, int oneline) {
    if(!styles)
        return NULL;

    String BUFFER = NewString("<style>");
    (!oneline ? BUFFER.AppendString(&BUFFER, "\n") : 0);
    int i = 0, css_idx = 0;

    for(int i = 0; styles[i] != NULL; i++) {
        if(styles[i]->Selector)
            BUFFER.AppendString(&BUFFER, ".");

        BUFFER.AppendArray(&BUFFER, (const char *[]){styles[i]->Class, " {", (!oneline ? "\n\n" : NULL), NULL});

        for(int css_idx = 0; (const char **)styles[i]->Data[css_idx] != NULL; css_idx++) {
            BUFFER.AppendArray(&BUFFER, (const char *[]){styles[i]->Data[css_idx], ";", NULL});;
        }
        
        (!oneline ? BUFFER.AppendString(&BUFFER, "\n") : 0);
        BUFFER.AppendString(&BUFFER, "}");
        (!oneline ? BUFFER.AppendString(&BUFFER, "\n") : 0);
    }

    BUFFER.AppendString(&BUFFER, "</style>");
    (!oneline ? BUFFER.AppendString(&BUFFER, "\n") : 0);
    BUFFER.data[BUFFER.idx] = '\0';
    
    char *BUFF = strdup(BUFFER.data);
    BUFFER.Destruct(&BUFFER);

    return BUFF;
}