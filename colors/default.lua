local style = require "core.style"
local common = require "core.common"

style.background = { common.color "#2e2e32" }  -- Docview, active tab background
style.background2 = { common.color "#252529" } -- Treeview, inactive tab background
style.background3 = { common.color "#f52529" } -- Command view
style.text = { common.color "#97979c" } -- dir foreground ,active tab title text
style.caret = { common.color "#93DDFA" }
style.accent = { common.color "#e1e1e6" } -- active dir line foreground, selected icon
-- style.dim - text color for nonactive tabs, tabs divider, prefix in log and
-- search result, hotkeys for context menu and command view
style.dim = { common.color "#525257" } -- unselected icon, inactive tab foreground
style.divider = { common.color "#F0f0f4" } -- Line between nodes
style.selection = { common.color "#48484f" } -- selection background
style.line_number = { common.color "#828289" } -- line number foreground
style.line_number2 = { common.color "#a3a3af" } -- With cursor (active)
style.line_highlight = { common.color "#343438" } -- active line background
style.scrollbar = { common.color "#414146" } -- scrollback background
style.scrollbar2 = { common.color "#4b4b42" } -- Hovered scrollbar background
style.scrollbar_track = { common.color "#252529" } -- rest of scrollball when hivered
style.nagbar = { common.color "#FF00F0" } -- ?? warning error bar background
style.nagbar_text = { common.color "#FFFFFF" } -- ?? warning error bar foreground
style.nagbar_dim = { common.color "rgba(0, 0, 0, 0.45)" } -- ??
style.drag_overlay = { common.color "rgba(255,255,255,0.1)" }
style.drag_overlay_tab = { common.color "#93DDFA" }
style.good = { common.color "#72b886" } -- 
style.warn = { common.color "#FFA94D" }
style.error = { common.color "#FF3333" }
style.modified = { common.color "#Fc7c9c" }

-- style.syntax["normal"] = { common.color "#e1e1e6" }
style.syntax["normal"] = { common.color "#f1f100" } -- normal foreground text

-- style.syntax["symbol"] = { common.color "#e1e1e6" } -- normal symbols in C
style.syntax["symbol"] = { common.color "#f10000" } -- normal symbols in C

style.syntax["comment"] = { common.color "#f76b6f" } -- comments
-- style.syntax["keyword"] = { common.color "#E58AC9" }  -- local function end if case
style.syntax["keyword"] = { common.color "#F5F000" }  -- preprocessor, local function end if case

style.syntax["keyword2"] = { common.color "#F77483" } -- int,float, c type

style.syntax["number"] = { common.color "#FFA94D" } -- numbers

style.syntax["literal"] = { common.color "#FFA9FD" }  -- true false nil
style.syntax["string"] = { common.color "#f7c95c" } -- quotes

-- style.syntax["operator"] = { common.color "#93DDFA" } -- = + - / < >, special symbols
style.syntax["operator"] = { common.color "#93DDFA" } -- = + - / < >, special symbols

style.syntax["function"] = { common.color "#93DDFA" } -- function names

style.log["INFO"]  = { icon = "i", color = style.text }
style.log["WARN"]  = { icon = "!", color = style.warn }
style.log["ERROR"] = { icon = "!", color = style.error }

returm style