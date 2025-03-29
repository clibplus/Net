<div align="center">
    <h1>Websign</h1>
    <p>Web Designing library for C</p>
</div>

### Initializing a Control

<details><summary>Control struct and CreateControl function Definition</summary>

```c
// This Control struct exmaple only include whats needed, you can use stack or heap at your choice 
// Destruct is only if any char * has memory allocated to them
typedef struct Control {
    void                *Parent;
    ControlTag          Tag;            // ControlTag
    char                *ID;
    char                *Name;
    char                *Type;          // Type for <input> <button> <select> <script>
    char                *Text;          // text for tags: <p> <h1> <h2> <h3>
    char                *Class;         // class=""
    char                *href;          // href for <a>
    char                **CSS;          // CSS Function Generator for the tag <div style="CSS FUNCTION"></div>
    long                CSSCount;
    char                *Data;          // For Any Other Data In the Opening Tag <div Data... > </div>
    char                *Script;
    int                 InlineJS;
    void                **SubControls;
    long                SubControlCount;
} Control;

Control *CreateControl(ControlTag tag, const char *sclass, const char *id, const char *text, Control **subcontrols);
```
</details>

<details><summary>Example</summary>

```c
Control *new_control = CreateControl(P_TAG, "new_css_selector", "new_label_id", "Hello World", NULL);
if(!new_control) {
    printf("[ - ] Error, Unable to create control....!\n");
    return 1;
}
```
</details>

### Create a pre-made template 

<details><summary>Example</summary>

```c
Control head = (Control){ .Tag = HEAD_TAG, .SubControlCount = 1, .SubControls = (void *[]){
    &(Control){ .Tag = TITLE_TAG, .Text = "Hello World Web-app" },
    NULL
}};

Control body = (Control){ .Tag = BODY_TAG, .CSSCount = 2, .CSS = (char *[]){"background-color: #000", "color: #fff", NULL}, .SubControls = (void *[]){
    &(Control){ .Tag = P_TAG, .Text = "Hello World from C :)" },
    NULL
}};

Control *IndexPage[] = {
    &head,
    &body,
    NULL
};
```
</details>

### Construct a template

<details><summary>Doc</summary>
```c
char *ConstructTemplate(Control **controls, CSS **styles, int click, int hover, int mouse_track, int keydown, int keyup, int oneline);
```
</details>

<details><summary>Example</summary>
```c
char *template = ConstructTemplate(IndexPage, NULL, 0, 0, 0, 0, 0, 0);
```
</details>