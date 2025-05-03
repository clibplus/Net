["click"].forEach(eventType => {
    document.addEventListener(eventType, async event => {
      try {
        const eventData = {
          Route: location.pathname,
          eventType: event.type,
          targetTag: event.target?.tagName || null,
          targetId: event.target?.id || null,
          targetClass: event.target?.className || null,
          targetText: event.target?.innerText || event.target?.value || null,
          timestamp: new Date().toISOString(),
          pageX: event.pageX ?? null,
          pageY: event.pageY ?? null,
          window_width: window.innerWidth,
          window_height: window.innerHeight
        };

        if (event.target.tagName === "INPUT" && event.target.getAttribute("type") === "submit") {
            eventData.form_data = Object.fromEntries(new FormData(event.target));
        }
  
        const response = await fetch("/ws_js_handler", {
          method: "POST",
          headers: {
            "Content-Type": "application/json"
          },
          body: JSON.stringify(eventData)
        });
  
        // Get the response text
        const rawText = await response.text();
        if (rawText === "fetched") return;
        if (!response.ok) return;
        const data = JSON.parse(rawText);
        if (data) {
          if (data.new_head_content) {
            document.head.innerHTML = data.new_head_content;
          }
  
          if (data.new_body_content) {
            document.body.innerHTML = data.new_body_content;
          }
  
          if (data.js_content) {
            eval(data.new_script_content);
          }
  
          if (data.new_header_content) {
            let header = document.querySelector("header");
            if (header) header.innerHTML = data.new_header_content;
          }
  
          if (data.new_footer_content) {
            let footer = document.querySelector("footer");
            if (footer) footer.innerHTML = data.new_footer_content;
          }

  
          if (data.change_values) {
            Object.entries(data.change_values).forEach(([id, content]) => {
              let el = document.getElementById(id);
              if (el) el.innerHTML = content;
            });
          }
  
          if (data.replace_elements) {
            Object.entries(data.replace_elements).forEach(([id, content]) => {
              let el = document.getElementById(id);
              if (el) el.outerHTML = content;
            });
          }

          if (data.resize_window) {
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
      } catch (err) {
        console.error("Error:", err);
        console.log("Error: " + err.message);
      }
    });
  });
  