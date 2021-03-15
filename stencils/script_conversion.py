#!/usr/bin/python
import sys
import re

if len(sys.argv) == 3:
    in_file = sys.argv[1]
    in_name = sys.argv[2]

    with open(in_file + '.html', 'r') as f_template:
        f_wrapper = '/* ' + in_name + '.h */\n\n'
        f_wrapper += '#ifndef ' + in_name.upper() + '_H\n'
        f_wrapper += '#define ' + in_name.upper() + '_H\n\n'
        f_wrapper += '#include <string>\n\n'
        f_wrapper += 'static const std::string ' + in_name + ' = "'

        f_content = f_template.read()
        f_content = f_content.replace('"', '\\"')
        f_content = f_content.replace('\n', '')
        f_content = f_content.replace('\t', '')
        f_content = re.sub(' {2,}', ' ', f_content)
        f_wrapper += f_content

        f_wrapper += '";\n\n'
        f_wrapper += '#endif ' + in_name.upper() + ' // ' + in_name.upper() + '_H\n'

        with open(in_name + '.h', 'w') as f_header:
            f_header.write(f_wrapper)
        
        f_header.close()
    
    f_template.close()
else:
    print('expected 2 arguments, received ' + str(len(sys.argv)))
