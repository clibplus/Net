#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../web.h"

char *JS_HANDLERS = ".forEach(eventType => document.addEventListener(eventType, async event => { try { const eventData = { Route: location.pathname, eventType: event.type, targetTag: event.target?.tagName || null, targetId: event.target?.id || null, targetClass: event.target?.className || null, timestamp: new Date().toISOString(), pageX: event.pageX ?? null, pageY: event.pageY ?? null, window_width: window.innerWidth, window_height: window.innerHeight }; const response = await fetch(\"/ws_js_handler\", { method: \"POST\", headers: { \"Content-Type\": \"application/json\" }, body: JSON.stringify(eventData) }); const rawText = await response.text(); if (rawText == \"fetched\") return; if (!response.ok) return; const data = JSON.parse(rawText); if (data) { if (data.new_head_content) document.head.innerHTML = data.new_head_content; if (data.new_body_content) document.body.innerHTML = data.new_body_content; if (data.new_script_content) { let scriptTag = document.querySelector(\"script[data-dynamic]\") || (() => { let s = document.createElement(\"script\"); s.setAttribute(\"data-dynamic\", \"true\"); document.body.appendChild(s); return s; })(); scriptTag.innerHTML = data.new_script_content; } if (data.new_header_content) { let header = document.querySelector(\"header\"); if (header) header.innerHTML = data.new_header_content; } if (data.new_footer_content) { let footer = document.querySelector(\"footer\"); if (footer) footer.innerHTML = data.new_footer_content; } if (data.replace_elements) Object.entries(data.replace_elements).forEach(([id, content]) => { let el = document.getElementById(id); if (el) el.innerHTML = content; }); if (data.resize_window) { window.innerWidth = data.resize_window[0]; window.innerHeight = data.resize_window[1]; } if (data.update_styles) Object.entries(data.update_styles).forEach(([selector, styles]) => { document.querySelectorAll(selector).forEach(el => Object.assign(el.style, styles)); }); } } catch (err) { console.error(\"Error:\", err); console.log(\"Error: \" + err.message); } }));";

// [\"click\", \"mouseover\"]
String ConstructHandler(int click, int hover, int mouse_track, int keyboard) {
    if(!click && !hover && !mouse_track && !keyboard)
        return ((String){0});

    String events = NewString("[\"");

    if(click) {
        events.AppendString(&events, "click\"");
    }

    if(hover) {
        (events.idx > 2 ? events.AppendString(&events, ", "): 0);
        events.AppendString(&events, "\"mouseover\"");
    }

    if(mouse_track) {
        (events.idx > 2 ? events.AppendString(&events, ", "): 0);
        events.AppendString(&events, "\"mousemove\"");
    }

    if(keyboard) {
        (events.idx > 2 ? events.AppendString(&events, ", "): 0);
        events.AppendString(&events, "\"keyup\"");
        events.AppendString(&events, ", ");
        events.AppendString(&events, "\"keydown\"");
    }

    events.AppendArray(&events, (const char *[]){"]", JS_HANDLERS, NULL});
    events.data[events.idx] = '\0';
    
    return events;
}

/*
{
    {"element_id", "new_content"}
}
*/
String ChangeElement(int count, char **arr) {
    String new_content = NewString("{\"replace_element\": {");

    for(int i = 0; i < count; i++)
    {
        if(arr[i] == NULL)
            break;

        if(arr[i][0] != NULL)
            new_content.AppendArray(&new_content, (const char *[]){"\"", (char *)arr[i][0], "\":\"", (char *)arr[i][1], "\"", NULL});

        if(arr[i + 1][0] != NULL)
            new_content.AppendString(&new_content, ",");
    }
    
    new_content.AppendString(&new_content, "}}");
    new_content.data[new_content.idx] = '\0';

    return new_content;
}