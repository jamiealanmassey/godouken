/* godouken_stencil_class.h */

#ifndef GODOUKEN_STENCIL_CLASS_H
#define GODOUKEN_STENCIL_CLASS_H

#include <string>

static const std::string godouken_stencil_class = "<html><head><title>{{ data.project.title }} {% if data.script.name != \"\" %} - {{ data.script.name }} {% endif %}</title><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"><link rel=\"stylesheet\" href=\"./../css/main.css\"></head><body><div class=\"container-fluid h-100\" id=\"main-content\"><div class=\"row h-100\"><div class=\"col-2 h-100 d-flex flex-column p-0\" id=\"sidebar\"><div id=\"logo-container\" class=\"container-fluid p-1\"><div class=\"mb-2\"><div class=\"mt-3 text-center\"><strong class=\"project-header border-bottom\">{{ data.project.title }}</strong></div></div></div><div id=\"sidebar-nav\" class=\"container-fluid p-0 scroll-y h-100\"><iframe src=\"generic/sidebar.html\" width=\"100%\" height=\"100%\" style=\"border: 0;\"></iframe></div></div><div class=\"col-10 h-100 d-flex flex-column scroll-y\"><div class=\"my-3\"><nav aria-label=\"breadcrumb\"><ol class=\"breadcrumb\">{% for crumb in data.script.breadcrumbs %}<li class=\"breadcrumb-item\"><a href=\"#\">{{ crumb }}</a></li>{% endfor %}</ol></nav><div id=\"content-script-tree\" class=\"container mt-5 text-center\"><div class=\"alert alert-secondary\" role=\"alert\"><strong>Script Inheritance Tree</strong></div></div><div id=\"content-title\" class=\"container\"><div class=\"row justify-content-center\"><h2 class=\"my-0 bounceIn\"><strong>{{ data.script.name }}</strong></h2></div><div class=\"row justify-content-center mt-3\">{% if data.script.meta.deprecated != \"\" %}<div class=\"ml-2 d-flex text-white\"><div class=\"widget bg-danger rounded-left mx-0\">deprecated</div><div class=\"widget bg-dark rounded-right mx-0\">{{ data.script.meta.deprecated }}</div></div>{% endif %}{% if data.script.meta.version != 0 %}<div class=\"ml-2 d-flex text-white\"><span class=\"widget bg-info rounded-left mx-0\">version</span><span class=\"widget bg-dark rounded-right mx-0\">{{ data.script.meta.version }}</span></div>{% endif %}{% if data.script.meta.created != \"\" %}<div class=\"ml-2 d-flex text-white\"><span class=\"widget bg-success rounded-left mx-0\">created</span><span class=\"widget bg-dark rounded-right mx-0\">{{ data.script.meta.created }}</span></div>{% endif %}{% if data.script.meta.modified != \"\" %}<div class=\"ml-2 d-flex text-white\"><span class=\"widget bg-success rounded-left mx-0\">last modified</span><span class=\"widget bg-dark rounded-right mx-0\">{{ data.script.meta.modified }}</span></div>{% endif %}{% if data.script.meta.collection != \"\" %}<div class=\"ml-2 d-flex text-white\"><span class=\"widget bg-warning rounded-left mx-0\">collection</span><span class=\"widget bg-dark rounded-right mx-0\">{{ data.script.meta.collection }}</span></div>{% endif %}</div>{% if data.script.description.brief != \"\" %}<div class=\"description\"><div class=\"row mt-4\"><h3 class=\"special-1\">Brief Description</h3></div><div class=\"row border-left border-info border-3 px-2 mt-2\"><i>{{ data.script.description.brief }}</i></div></div>{% endif %}{% if data.script.description.detailed != \"\" %}<div class=\"description\"><div class=\"row mt-4\"><h3 class=\"special-1\">Detailed Description</h3></div><div class=\"row border-left border-info border-3 px-2 mt-2\"><i>{{ data.script.description.detailed }}</i></div></div>{% endif %}</div>{% if length(data.script.properties) > 0 %}<div id=\"content-main-body\" class=\"container my-3\"><div class=\"row\"><h5 class=\"special-1\">Script Properties</h5><table class=\"table table-sm mt-3\"><thead><tr><td><strong>Property Type</strong></td><td><strong>Property Name</strong></td><td><strong>Special Tags</strong></td></tr></thead><tbody>{% for property in data.script.properties %}<tr><td>{% if property.type_info.href %}<a href=\"{{ property.type_info.href }}\" target=\"_blank\">{{ property.type_info.name }}</a>{% else %}{{ property.type_info.name }}{% endif %}</td><td><a href=\"#{{ property.name }}\">{{ property.name }}</a></td>{% if property.tags %}<td>{% if property.tags.is_godot %}<div class=\"d-flex text-white\"><span class=\"widget bg-dark rounded mx-0 px-2\">godot type</span></div>{% endif %}{% if property.tags.is_exported %}<div class=\"d-flex text-white\"><span class=\"widget bg-dark rounded mx-0 px-2\">exports // {{ property.type_info.name }}</span></div>{% endif %}</td>{% endif %}</tr>{% endfor %}</tbody></table></div></div>{% endif %}{% if length(data.script.properties) %}<div id=\"content-main-body-detailed\" class=\"container my-3\"><div class=\"row mt-3\"><h3 class=\"special-1\">Property Descriptions</h3><div class=\"method-descriptor container-fluid border-left border-success border-3 bg-light mt-3\">{% for property in data.script.properties %}<div class=\"container-fluid border-left border-info border-3 my-3 ml-2 py-2\"><div class=\"row ml-2 mt-2 d-flex flex-column\"><span class=\"special-2\"><i><strong>Property Signature</strong></i></span><div><h6 class=\"my-0\">{% if property.type_info.href %}<a href=\"{{ property.type_info.href }}\" target=\"_blank\">{{ property.type_info.name }}</a>&nbsp;{% else %}{{ property.type_info.name }}&nbsp;{% endif %}{{ property.name }}</h6>{% if property.description.brief %}<p>{{ property.description.brief }}</p>{% endif %}{% if property.description.detailed %}<p>{{ property.description.detailed }}</p>{% endif %}</div></div></div>{% endfor %}</div></div></div>{% endif %}</div></div></div></div><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script><script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script><script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script></body></html>";

#endif GODOUKEN_STENCIL_CLASS // GODOUKEN_STENCIL_CLASS_H
