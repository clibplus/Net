#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "web.h"

char *JS_HANDLER_FORMAT = "<script src=\"ws_form_handler.js\"></script><br /><script>document.getElementById('[SUBMIT_BUTTON]').addEventListener('click', () => submitForm('[FORM_ID]'));\n</script>";
char *JS_HANDLER[] = {"(()=>(f=new FormData(document.getElementById('", "')),d={},f.forEach((v,k)=>d[k]=v),fetch(window.location.href,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(d)}).then(r=>r.json()).then(r=>{console.log('Response:',r);alert('POST Request successful!')}).catch(e=>{console.error('Error:',e);alert('POST Request failed!')})))();"};

String ConstructOnClickForm(Control *p) {
    if(p->OnClick || p->FormID)
        return ((String){});

    String JS_CODE = NewString(JS_HANDLER[0]);
    JS_CODE.AppendArray(&JS_CODE,  (const char *[]){p->FormID, JS_HANDLER[1], NULL});

    JS_CODE.data[JS_CODE.idx] = '\0';
    return JS_CODE;
}

// Constryct 
String ConstructJS(WJS *js) {
    switch((int)js->Action) {
        case REDIRECT: {

        }
        case MSG_BEFORE_REDIRECT: {

        }
        case GET_RESULTS: {

        }
        case SPIN_UNTIL_RESULTS: {

        }
        case VERTICAL_BOOMERANG_UNTIL_RESULT: {

        }
        case HORIZONTAL_BOOMERANG_UNTIL_RESULT: {

        }
    }
}