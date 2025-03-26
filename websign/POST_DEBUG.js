document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll("form[method='post']").forEach(form => {
        form.addEventListener("submit", async event => {
            event.preventDefault(); // Prevent default submission

            const formData = new FormData(form);

            try {
                const response = await fetch(form.action, {
                    method: "POST",
                    body: formData
                });

                const rawText = await response.text();
                if(rawText == "fetched") return;

                if (!response.ok) {
                    return;
                }

                const data = JSON.parse(rawText);
                if (data) {
                    if (data.new_head_content) {
                        document.head.innerHTML = data.new_head_content;
                    }

                    if (data.new_body_content) {
                        document.body.innerHTML = data.new_body_content;
                    }

                    if (data.new_header_content) {
                        let header = document.querySelector("header");
                        if (header) header.innerHTML = data.new_header_content;
                    }

                    if (data.new_footer_content) {
                        let footer = document.querySelector("footer");
                        if (footer) footer.innerHTML = data.new_footer_content;
                    }

                    if (data.change_value) {
                        Object.entries(data.replace_elements).forEach(([id, content]) => {
                            let el = document.getElementById(id);
                            if (el) { el.innerHTML = content; }
                        });
                    }

                    if (data.replace_elements) {
                        Object.entries(data.replace_elements).forEach(([id, content]) => {
                            let el = document.getElementById(id);
                            if (el) {
                                el.outerHTML = content; // Completely replaces the element with the new one
                            }
                        });
                    }

                    if(data.resize_window) {
                        window.innerWidth = data.resize_window[0];
                        window.innerHeight = data.resize_window[1];
                    }

                    if (data.update_styles) {
                        Object.entries(data.update_styles).forEach(([selector, styles]) => {
                            document.querySelectorAll(selector).forEach(el => {
                                Object.assign(el.style, styles);
                            });
                        });
                    }
                }
            } catch (error) {
                console.error("Form submission error:", error);
            }
        });
    });
});