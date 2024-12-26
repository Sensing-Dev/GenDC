import urllib.request
import os
from pathlib import Path
from pypdf import PdfReader
import json

src_dir = os.path.dirname(__file__)
data_pdf = os.path.join(src_dir, 'data.pdf')
pdf_url = 'https://www.emva.org/wp-content/uploads/GenICamPixelFormatValues.pdf'

root_dir = Path(src_dir).parent.absolute()

python_dir = os.path.join(root_dir, 'gendc_python', 'gendc_separator')
cpp_dir = os.path.join(root_dir, 'gendc_cpp', 'gendc_separator')

if __name__ == "__main__":
    try:
        urllib.request.urlretrieve(pdf_url, data_pdf)
    except:
        print('Failed to DL {}'.format(pdf_url))
        exit(1)

    data_int_key = {}
    data_str_key = {}
    pdf_date = ''

    try:
        reader = PdfReader(data_pdf)
        for page in reader.pages:
            lines = page.extract_text().split('\n')
            for line in lines:
                columns = line.split(' ', 3)
                if len(columns) == 4 and columns[1].startswith('0x'):
                    data_int_key[int(columns[1], 16)] = columns[0]
                    data_str_key[columns[0]] = int(columns[1], 16)
                elif len(columns) == 1 and len(columns[0].split('-'))==3:
                    pdf_date = columns[0]
                    print('line \"{}\" is skipped to convert'.format(line))
                else:
                    print('line \"{}\" is skipped to convert'.format(line))
    except:
        print('Failed to load {}'.format(data_pdf))
        exit(1)

    try:
        for filename in ['data_int_key', 'data_str_key']:
            with open(os.path.join(python_dir, '{}.py'.format(filename)), 'w') as f:
                cont = '# {}.py generated based on {} [{}]\n\n'.format(filename, pdf_url, pdf_date)
                cont += filename + ' = { \\\n'
                for key in eval(filename):
                    if filename == 'data_int_key':
                        cont += '    {} : \'{}\',\n'.format(key, eval(filename)[key])
                    elif filename == 'data_str_key':
                        cont += '    \'{}\' : {},\n'.format(key, eval(filename)[key])
                cont += '}'
                f.write(cont)

            with open(os.path.join(cpp_dir, '{}.h'.format(filename)), 'w') as f:
                cont = '#ifndef {}_H\n'.format(filename.upper())
                cont += '#define {}_H\n'.format(filename.upper())
                cont += '// {}.h generated based on {} [{}]\n\n'.format(filename, pdf_url, pdf_date)
                cont += '#include <iostream>\n'
                cont += '#include <map>\n'
                cont += '#include <string>\n\n'
                if filename == 'data_int_key':
                    cont += 'std::map<int32_t, std::string> ' + filename + ' {\n'
                elif filename == 'data_str_key':
                    cont += 'std::map<std::string, int32_t> ' + filename + ' {\n'
                for key in eval(filename):
                    if filename == 'data_int_key':
                        cont += '    {' + str(key) + ':\"' + eval(filename)[key] + '\"},\n'
                    elif filename == 'data_str_key':
                        cont += '    {\"' + key + '\":' + str(eval(filename)[key]) + '},\n'
                cont += '};\n\n'
                cont += '#endif /*{}_H*/'.format(filename.upper())
                f.write(cont)
                
    except:
        print('Failed to save {}'.format(filename))
        exit(1)

    print(pdf_date) 

