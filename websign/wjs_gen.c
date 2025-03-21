#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../web.h"

char *JS_HANDLERS = ".forEach(e=>document.addEventListener(e,async t=>{try{let a={Route:location.pathname,eventType:t.type,targetTag:t.target?.tagName||null,targetId:t.target?.id||null,targetClass:t.target?.className||null,timestamp:new Date().toISOString(),pageX:t.pageX??null,pageY:t.pageY??null,window_width:innerWidth,window_height:innerHeight},n=await fetch(\"/ws_js_handler\",{method:\"POST\",headers:{\"Content-Type\":\"application/json\"},body:JSON.stringify(a)}),o=await n.text();if(console.log(\"Server Response: \"+o),!n.ok)return void console.error(\"Server error:\",n.status,o);let r=JSON.parse(o);if(console.log(\"Received response:\",r),r){if(r.new_head_content&&(document.head.innerHTML=r.new_head_content),r.new_body_content&&(document.body.innerHTML=r.new_body_content),r.new_script_content){let d=document.querySelector(\"script[data-dynamic]\")||(()=>{let c=document.createElement(\"script\");return c.setAttribute(\"data-dynamic\",\"true\"),document.body.appendChild(c),c})();d.innerHTML=r.new_script_content}r.new_header_content&&(document.querySelector(\"header\").innerHTML=r.new_header_content),r.new_footer_content&&(document.querySelector(\"footer\").innerHTML=r.new_footer_content),r.replace_elements&&Object.entries(r.replace_elements).forEach(([d,c])=>{let s=document.getElementById(d);s?s.innerHTML=c:console.warn(`Element with id \"${d}\" not found.`)}),r.resize_window&&(innerWidth=r.resize_window[0],innerHeight=r.resize_window[1]),r.update_styles&&Object.entries(r.update_styles).forEach(([d,c])=>{document.querySelectorAll(d).forEach(s=>Object.assign(s.style,c))})}}catch(a){console.error(\"Error:\",a),console.log(\"Error: \"+a.message)}}));";

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