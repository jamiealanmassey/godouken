/* godouken_stencil_directory.h */

#ifndef GODOUKEN_STENCIL_DIRECTORY_H
#define GODOUKEN_STENCIL_DIRECTORY_H

#include <string>

static const std::string godouken_stencil_directory = "<html><head><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"><link rel=\"stylesheet\" href=\"./../../css/main.css\"></head><body><div class=\"container-fluid h-100\" id=\"main-content\"><div class=\"row h-100\"><div class=\"col-2 h-100 d-flex flex-column p-0\" id=\"sidebar\"><div id=\"logo-container\" class=\"container-fluid p-1\"><div class=\"mb-2\"><div class=\"mt-3 text-center\"><strong class=\"project-header border-bottom\"></strong></div></div></div><div id=\"sidebar\" class=\"container-fluid p-0 scroll-y h-100\"><iframe src=\"./../generic/sidebar.html\" class=\"scroll-y\" width=\"100%\" height=\"100%\" style=\"border: 0;\"></iframe></div></div><div class=\"col-10 h-100 d-flex flex-column scroll-y\"><div class=\"my-3\">{% if length(breadcrumbs) > 0 %}<nav aria-label=\"breadcrumb\"><ol class=\"breadcrumb\">{% for crumb in breadcrumbs %}<li class=\"breadcrumb-item\"><a href=\"{{ crumb.url }}\">{{ crumb.name }}</a></li>{% endfor %}</ol></nav>{% endif %}<div class=\"container mt-5 text-left\">{% if length(entries.dirs) > 0 %}<div class=\"row\"><div class=\"col\"><h3><b>Directories</b></h3></div></div><div class=\"row mb-5\"><div class=\"col\">{% for dir in entries.dirs %}<div><a href=\"{{ dir.url }}\">{{ dir.name }}</a></div>{% endfor %}</div></div>{% endif %}{% if length(entries.abc) > 0 %}<div class=\"row\">{% for container in entries.abc %}<div class=\"col-xs-12 col-md-6 col-lg-4 mb-3\"><h3><b>{{ container.alpha }}</b></h3>{% for entry in container.files %}<div><a href=\"../{{ entry.name_html }}.html\">{{ entry.name }}</a></div>{% endfor %}</div>{% endfor %}</div>{% endif %}</div></div></div></div></div><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script><script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script><script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script></body></html>";

#endif GODOUKEN_STENCIL_DIRECTORY // GODOUKEN_STENCIL_DIRECTORY_H
