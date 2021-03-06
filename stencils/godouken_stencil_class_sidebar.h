/* godouken_stencil_class_sidebar.h */

#ifndef GODOUKEN_STENCIL_CLASS_SIDEBAR_H
#define GODOUKEN_STENCIL_CLASS_SIDEBAR_H

#include <string>

static const std::string godouken_stencil_class_sidebar = "<html> <head><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"><link rel=\"stylesheet\" href=\"./../../css/main.css\"></head> <body> <div id=\"sidebar-nav\" class=\"h-100 scroll-y\"> <ul id=\"sidebar\" class=\"h-100 scroll-y\"> <li class=\"tree-item-heading\">OVERVIEW</li> <li class=\"tree-item\"><a href=\"#\" onclick=\"set_location('index');\">Project Index</a></li> {% if data.reference %} <li class=\"tree-item\"><a href=\"#\" onclick=\"set_location('reference');\">Reference Manual</a></li> {% endif %} <li class=\"tree-item-heading\">SCRIPT API</li> {% for entry in data.scripts %} <li class=\"tree-item\"> <a href=\"#\" onclick=\"set_location('{{ entry.name_html }}');\">{{ entry.name_clean }}</a> </li> {% endfor %} </ul> </div> <script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script><script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script><script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script> <script> function set_location(href) { top.location = \"./../\" + href + \".html\"; } </script> </body></html>";

#endif GODOUKEN_STENCIL_CLASS_SIDEBAR // GODOUKEN_STENCIL_CLASS_SIDEBAR_H
