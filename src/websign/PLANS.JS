
// Need to be adjusted first 
// This will change <script> tags but need an attribute for unique names
if (data.new_script_content) {
    let scriptTag = document.querySelector("script[data-dynamic]") || (() => {
        let s = document.createElement("script");
        s.setAttribute("data-dynamic", "true");
        document.body.appendChild(s);
        return s;
    })();

    scriptTag.innerHTML = data.new_script_content;
}