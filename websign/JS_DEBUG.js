["click", "mousemove"].forEach(eventType => {
    document.addEventListener(eventType, async event => {
        try {
            const eventData = {
                eventType: event.type,
                targetTag: event.target.tagName,
                Route: location.href,
                targetId: event.target.id || null,
                targetClass: event.target.className || null,
                timestamp: new Date().toISOString(),
                pageX: event.pageX,
                pageY: event.pageY,
                window_width: window.innerWidth,
                window_height: window.innerWidth
            };

            const response = await fetch("/event_handler", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(eventData)
            });

            if (!response.ok) {
                return console.error("Server error:", response.status);
            }

            const data = await response.json();
            if (data) {
                // Update <head> content if provided
                if (data.new_head_content) {
                    document.head.innerHTML = data.new_head_content;
                }

                // Update <body> content if provided
                if (data.new_body_content) {
                    document.body.innerHTML = data.new_body_content;
                }

                // Inject new script content dynamically
                if (data.new_script_content) {
                    let scriptTag = document.querySelector("script[data-dynamic]") || (() => {
                        let s = document.createElement("script");
                        s.setAttribute("data-dynamic", "true");
                        document.body.appendChild(s);
                        return s;
                    })();
                    scriptTag.innerHTML = data.new_script_content;
                }

                // Update <header> content if provided
                if (data.new_header_content) {
                    let header = document.querySelector("header");
                    if (header) header.innerHTML = data.new_header_content;
                }

                // Update <footer> content if provided
                if (data.new_footer_content) {
                    let footer = document.querySelector("footer");
                    if (footer) footer.innerHTML = data.new_footer_content;
                }

                // Replace specific elements' content by ID
                if (data.replace_elements) {
                    Object.entries(data.replace_elements).forEach(([id, content]) => {
                        let el = document.getElementById(id);
                        if (el) el.innerHTML = content;
                    });
                }

                if(data.resize_window) {
                    window.innerWidth = data.resize_window[0];
                    window.innerHeight = data.resize_window[1];
                }
            }
        } catch (err) {
            console.error("Error:", err);
        }
    });
});
